# Architecture-Guided Test Resource Allocation Via Logic
(TAP2021 Publication Supporting Data)

* Find the experimental data in the [`evaluation`](evaluation) subfolder, including the trigger scripts.
* The Python API and helper classes are located in [`pyASTRAHL`](pyASTRAHL). 
* Evaluation scripts and Jupyter notebooks are in [`notebooks`](notebooks)
* [`src`](src) contains the main tool source code (and the Makefile to build it)
* Below you can find some help for building and using it.

## Installing

- Install [cJSON](https://github.com/DaveGamble/cJSON)
- Clone this repository
- Go to the `src` directory
- Launch `make`

Install procedure cJSON:
```
git clone https://github.com/DaveGamble/cJSON.git
cd cJSON
mkdir -p build
cmake ..
make
make install
```

Install procedure gc
```
apt-get install -y autoconf automake libtool &> /dev/null

# this is the stuff for gc
git clone git://github.com/ivmai/bdwgc.git ; 
cd bdwgc ; 
git clone git://github.com/ivmai/libatomic_ops.git ;

# Build gc
cd bdwgc 
autoreconf -vif 
automake --add-missing 
./configure 
make
make install 
```

**For Python usage (convenience API)**
```
sudo apt-get install jq graphviz
pip install graphviz libgraphviz-dev joblib tqdm numpy pandas
```

## Using

Typical usage (not implemented yet):
```
./main -f input.json
```
(See "JSON format" section for more information.)

### Options

```
./main --test xxx
```
Launches a bunch of unit tests for a given file.
Unexpected results are printed in red

```
./main --test [all]
```
Launches a bunch of unit tests on all files.
Unexpected results are printed in red.

## JSON format

This tool uses JSON format to describe:
- the input fault tree,
- the confidence formulas for all components,
- the resources already spent on all components,
- the whole amount of resources to spend on the problem.

### Fault trees

Connectives:

- `wire`: describes a base wire (component), needs:

  - `index`: an integer for identification,

- `and`, `or`, `pand`: gates, need:

  - `subtree1`, `subtree2`: JSON objects representing the sub-fault trees.

For an example, see `fault_tree.json`.

### Expressions

Connectives:

- `const`: describes a constant, needs:

  - `constant`: a double,

- `var`: describsed a variable, needs:

  - `index`: an integer for identification,

  - `name`: a name for the variable (for printing),

  - WARNING: no check is made to verify the naming scheme is coherent,

- `opp`: unary operations, need:

  - `expression1`: JSON object representing the sub-expression.

- `add`, `sub`, `mul`, `div`: binary operations, need:

  - `expression1`, `expression2`: JSON objects representing the sub-expressions.

For an example, see `expression.json`.
