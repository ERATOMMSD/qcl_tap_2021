import math
import random
import graphviz
import json

import logging
logger = logging.getLogger(__name__)

def log_if_level(level, message):
    """Logs the message if the level is below the specified level"""
    if logger.getEffectiveLevel() <= level:
        logger.log(level, message)


        
#######################################
# Definition of Fault Trees
#######################################

class FaultTree(object):
    """Abstract, don't use !"""
    shape = "egg"
    
    def __init__(self, *subtrees, name=""):
        self.subtrees = subtrees
        self.name = name

    def to_json(self, wire_order=None):
        ret = {"type": self.tree_name, "name": self.name}
        for idx, subtree in enumerate(self.subtrees):
            ret["subtree" + str(idx + 1)] = subtree.to_json(wire_order)
        return ret

    def as_json(self):
        return json.dumps(self.to_json(), indent=2)
    
    def draw(self, values=None):
        values = values or dict()
        return graphviz.Source("digraph G {\n ranksep=0;\n " + self.to_dot(values) + "}")
    
    def to_dot(self, values):
        ret = []
#         ret.append(f"{id(self)} [label=\"{self.tree_name.upper()}\", shape=square];")
        ret.append("subgraph "+ str(id(self)) + " {")
        ret.append("color=invis;ranksep=0;")
        ret.append("label{} [label=\"{}\" style=filled fillcolor=\"#f2f2f2\" shape=box];".format(id(self), self.name))
        ret.append("gate{} [label=\"{}\" style=filled fillcolor=\"#D3D3D3\" shape={}];".format(id(self), self.tree_name.upper(), self.shape))
        ret.append("label{} -> gate{};".format(id(self), id(self)))
        ret.append("}")
        for subtree in self.subtrees:
            ret.append(subtree.to_dot(values))
            ret.append("gate{} -> label{} [minlen=1]".format(id(self), id(subtree)))
        return "\n".join(ret);

    @property
    def wires(self):
        return set().union(*[st.wires for st in self.subtrees])
    
    @classmethod
    def from_json(cls, encoded, node_dict=None):
        """Node dict stores wires that we already saw so we don't create the same wire twice"""
        _type = encoded["type"]
        decoders = {
            And.tree_name : And,
            Or.tree_name : Or,
            Pand.tree_name : Pand,
            Wire.tree_name : Wire
        }
        
        if node_dict is None:
            node_dict = dict()

        if _type not in decoders:
            raise ValueError("I don't recognize this type: {}".format(_type))
        
        return decoders[_type].from_json(encoded, node_dict)


def json_decode_tree(cls, encoded, node_dict):
    if node_dict is None:
        node_dict = dict()

    assert encoded["type"] == cls.tree_name
    name = encoded.get("name", "")
    subtrees = []
    for key, node in encoded.items():
        if key.startswith("subtree"):
            st = FaultTree.from_json(node, node_dict)
            subtrees.append(st)
    return cls(*subtrees, name=name)


class And(FaultTree):
    tree_name = "and"
    shape = "triangle"
    
    @classmethod
    def from_json(cls, encoded, node_dict=None):
        return json_decode_tree(cls, encoded, node_dict)


class Pand(FaultTree):
    tree_name = "pand"
    shape = "house"
    
    @classmethod
    def from_json(cls, encoded, node_dict=None):
        return json_decode_tree(cls, encoded, node_dict)


class Or(FaultTree):
    tree_name = "or"
    shape = "invtriangle"
    
    @classmethod
    def from_json(cls, encoded, node_dict=None):
        return json_decode_tree(cls, encoded, node_dict)


class BasicEvent(FaultTree):
    tree_name = "wire"

    def __init__(self, name=""):
        """
        Make sure every BasicEvent has a consecutive, unique index. The tool needs it.
        It would perhaps be smarter to use hash() or id(), but then the files don't look as
        """
        self.name = name
  
    def to_json(self, wire_order=None):
        idx = None
        if wire_order:
            idx = wire_order.index(self)
        return {"type": self.tree_name, "index": idx, "name": self.name}

    @classmethod
    def from_json(cls, encoded, node_dict):
        assert "name" in encoded
        assert encoded["type"] == cls.tree_name
        name = encoded["name"]

        if name not in node_dict:  # create the object only if it's not present in the dict yet
            node_dict[name] = cls(name)
        
        return node_dict[name]
    
    def to_dot(self, values):
        return "subgraph "+ str(id(self)) + " {" + """
color=invis;
label{0} [label=\"{1}\", style=filled, fillcolor=\"#f2f2f2\", shape=box];
gate{0} [label=\"{2}\", width=.5, fixedsize=true, fontsize=10, style=filled, fillcolor=\"#D3D3D3\", shape=circle];
label{0} -> gate{0};
""".format(id(self), self.name, values.get(self) if self in values else '') + "}"


    @property
    def wires(self):
        return {self}
    
    def __str__(self):
        return self.name
    
class Wire(BasicEvent):
    
    def __repr__(self):
        return self.name




#######################################
# Definition of Expressions
#######################################

class Expression(object):
    """Abstract base class, don't use"""
    def as_json(self, index=-1, wire_index=-1):
        return json.dumps(self.to_json(index, wire_index), indent=2)
    
    @staticmethod
    def from_json(encoded):
        _type = encoded["type"]
        decoders = {
            "var" : Var,
            "const" : Const,
            Neg.op_name : Neg,
            Log.op_name : Log,
            Add.op_name : Add,
            Sub.op_name : Sub,
            Mul.op_name : Mul,
            Div.op_name : Div,
            Pow.op_name : Pow,
        }
        if _type not in decoders:
            raise ValueError("I don't recognize this type: {}".format(_type))
        
        return decoders[_type].from_json(encoded)
        
    def _conv_other(self, other):
        """make sure to convert constants to a Const and Wires to Var"""
        if isinstance(other, (int, float)):
            return Const(other)
        if isinstance(other, Wire):
            return Var(other)
        return other
    
    def __add__(self, other):
        return Add(self, self._conv_other(other))
    
    def __radd__(self, other):
        return Add(self._conv_other(other), self)
    
    def __sub__(self, other):
        return Sub(self, self._conv_other(other))
    
    def __rsub__(self, other):
        return Sub(self._conv_other(other), self)
    
    def __mul__(self, other):
        return Mul(self, self._conv_other(other))
    
    def __rmul__(self, other):
        return Mul(self._conv_other(other), self)
    
    def __div__(self, other):
        return Div(self, self._conv_other(other))
    
    def __rdiv__(self, other):
        return Div(self._conv_other(other), self)
    
    def __truediv__(self, other):
        return Div(self, self._conv_other(other))
    
    def __rtruediv__(self, other):
        return Div(self._conv_other(other), self)
    
    def __mod__(self, other):
        return Mod(self, self._conv_other(other))
    
    def __rmod__(self, other):
        return Mod(self._conv_other(other), self)
    
    def __pow__(self, other):
        return Pow(self, self._conv_other(other))
    
    def __rpow__(self, other):
        return Pow(self._conv_other(other), self)
    
    def __neg__(self):
        return Neg(self)

class Const(Expression):
    def __init__(self, value):
        self.value = value
    
    def __call__(self, _):
        return self.value

    def to_json(self, index, wire_index):
        return {"type": "const", "constant": self.value }
    
    @staticmethod
    def from_json(encoded):
        constvalue = encoded["constant"]
        assert encoded["type"] == "const"
        assert isinstance(constvalue, (float, int))
        return Const(constvalue)
    
    def __str__(self):
        return "{:.4f}".format(self.value)

"""This is a constant for the Euler number e"""
EUL = Const(2.71828182845904523536028747135266249775724709369995)

class Var(Expression):
    def __init__(self):
        pass
        
    def __call__(self, wire_value):
        return wire_value

    def to_json(self, index, wire_index):
        return {
            "type": "var", 
            #  "name": str(self.wire),
            "index": index,  # the index of the 
            "wire_index": wire_index
        }
    
    @classmethod
    def from_json(cls, encoded):
        assert encoded["type"] == "var"
        return cls()

    def __str__(self):
        return "Var()"

class UnaryExpression(Expression):
    """ Abstract, don't use """
    def __init__(self, operand):
        self.operand = operand

    def to_json(self, index, wire_index):
        return {"type": self.op_name, "expression1": self.operand.to_json(index, wire_index)}

    @classmethod
    def from_json(cls, encoded):
        assert encoded["type"] == cls.op_name
        operand = Expression.from_json(encoded["expression1"])
        return cls(operand)
    
class Neg(UnaryExpression):
    op_name = "opp"
    
    def __call__(self, wire_value):
        return self.operand(wire_value) * -1

    def __str__(self):
        return "-({})".format(self.operand)

        
class Log(UnaryExpression):
    op_name = "log"
    
    def __call__(self, wire_value):
        return math.log(self.operand(wire_value))
    
    def __str__(self):
        return "log({})".format(self.operand)

class BinaryExpression(Expression):
    """ Abstract, don't use """

    def __init__(self, leftoperand, rightoperand):
        self.left = leftoperand
        self.right = rightoperand

    def to_json(self, index, wire_index):
        return {"type": self.op_name, 
                "expression1": self.left.to_json(index, wire_index), 
                "expression2": self.right.to_json(index, wire_index) 
               }
    
    @classmethod
    def from_json(cls, encoded):
        assert encoded["type"] == cls.op_name
        operand1 = Expression.from_json(encoded["expression1"])
        operand2 = Expression.from_json(encoded["expression2"])
        return cls(operand1, operand2)

class Add(BinaryExpression):
    op_name = "add"
    
    def __call__(self, wire_value):
        return self.left(wire_value) + self.right(wire_value)

    def __str__(self):
        return "({} + {})".format(self.left, self.right)
    
class Sub(BinaryExpression):
    op_name = "sub"
    
    def __call__(self, wire_value):
        return self.left(wire_value) - self.right(wire_value)
    
    def __str__(self):
        return "({} - {})".format(self.left, self.right)

class Mul(BinaryExpression):
    op_name = "mul"
    
    def __call__(self, wire_value):
        return self.left(wire_value) * self.right(wire_value)
    
    def __str__(self):
        return "{} * {}".format(self.left, self.right)

class Div(BinaryExpression):
    op_name = "div"
    
    def __call__(self, wire_value):
        return self.left(wire_value) / self.right(wire_value)
    
    def __str__(self):
        return "{} / ({})".format(self.left, self.right)

class Pow(BinaryExpression):
    op_name = "pow"
    
    def __call__(self, wire_value):
        return self.left(wire_value) ** self.right(wire_value)
    
    def __str__(self):
        return "{}^({})".format(self.left, self.right)
