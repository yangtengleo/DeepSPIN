# DeepSPIN
DeepSPIN is a method of constructing a deep learning potential for magnetic systems described in the paper [*Screening Spin Lattice Interaction Using Deep Learning Approach*](https://doi.org/10.48550/arXiv.2304.09606). DeepSPIN has been integrated into [DeePMD-kit](https://github.com/deepmodeling/deepmd-kit) as a new feature of the release version `v2.2.2`.

## How to cite
```
@article{yang2023screening,
  title={Screening Spin Lattice Interaction Using Deep Learning Approach},
  author={Yang, Teng and Cai, Zefeng and Huang, Zhengtao and Tang, Wenlong and Shi, Ruosong and Godfrey, Andy and Liu, Hanxing and Lin, Yuanhua and Nan, Ce-Wen and Zhang, LinFeng and others},
  journal={arXiv preprint arXiv:2304.09606},
  year={2023}
}
```

## Requirements
To train the DeepSPIN model, Python 3.7+ interpreter and following packages are required：
- deepmd-kit=2.2.2
- dpdata

In Linux, you can quickly achieve the requirements by running
```shell
# install miniconda with python 3.9
wget https://repo.anaconda.com/miniconda/Miniconda3-py39_4.10.3-Linux-x86_64.sh
bash Miniconda3-py39_4.10.3-Linux-x86_64.sh

# install packages with conda 
conda create -n deepmd deepmd-kit=2.2.2 -c conda-forge

# init environment
conda activate deepmd

# install packages with pip
pip install dpdata
```

## Demo: DeepSPIN study on antiferromagnetic NiO
The usage of DeepSPIN is similar to that of [DeePMD-kit](https://github.com/deepmodeling/deepmd-kit). The guide of a quick start on DeePMD-kit can be found [here](https://github.com/deepmodeling/deepmd-kit/blob/master/doc/getting-started/quick_start.ipynb). 

### Dataset preparation
The original training data should be sourced from the first-principle non-collinear magnetic excitation calculation method [DeltaSpin](https://github.com/caizefeng/DeltaSpin) based on VASP, in which the lattice configuration is collected from the input file `POSCAR`, the atomistic spin configuration is collected from the input file `INCAR`, the potential energy of system and atomic forces are collected from the output file `OUTCAR`, and magnetic forces are collected from the output file `OSZICAR`. Here we take the 32-atom $2\times2\times2$ NiO rhombohedral supercell for the calculation. 

After some preprocessing, the format of original data should be converted to fit the general input file requirements for DeePMD-kit, as shown in `example/NiO/raw`, more detailed illustration can be referred [here](https://github.com/deepmodeling/deepmd-kit/blob/master/doc/data/index.md). It's worth noting that the type of pseudo atoms around Ni atoms is represented by type 2 in `type.raw`, the Cartesian coordinates of pseudo atoms around Ni atoms are stitched after the coordinates of real atoms sequentially by index in `coord.raw`, and the magnetic forces of Ni atoms are stitched after the atomic forces of real atoms sequentially as well in `force.raw`. 

Then the data should be split into training set and validation set, converted from `.raw` format to `.npy` format using the script `raw_to_set.sh` in `example/NiO/scripts`. For example, we have 100 frames in `example/NiO/raw`,
```shell
$ cd example/NiO/raw
$ sh ../scripts/raw_to_set.sh 20
nframe is 100
nline per set is 20
will make 5 sets
making set 0 ...
making set 1 ...
making set 2 ...
making set 3 ...
making set 4 ...
$ ls
box.raw  coord.raw  energy.raw  force.raw  set.000  set.001  set.002 set.003 set.004 type.raw
```
It generates 5 subsets from `set.000` to `set.004`, with each subset containing 20 frames. The former 4 subsets will be picked as training set, and the last one will be validation set, as shown in `example/NiO/data`. 

### Prepare input script
Once the dataset preparation is complete, a `json` format input script is required to specify the parameters for training. Here we take `example/NiO/NiO_spin.json` as an example, and parameters related to DeepSPIN are introduced as following. Detailed information about other parameters can refer to the [documentation](https://github.com/deepmodeling/deepmd-kit/tree/master/doc/model).

#### spin
The hyper-parameters related to spin is set in the following section
```json
    "spin" : {
        "use_spin":         [true, false],
        "virtual_len":      [0.4],
        "spin_norm":        [1.2737],
    },
```
* `use_spin` determines whether the atom type is magnetic.
* `virtual_len` specifies the distance between the pseudo atom and its corresponding real atom.
* `spin_norm` specifies the magnitude of the magnetic moment for each magnatic atom.

#### loss
The loss function for the training is set by the section
```json
  "loss" : {
      "type":               "ener_spin",
      "start_pref_e":       0.02,
      "limit_pref_e":       1,
      "start_pref_fr":      1000,
      "limit_pref_fr":      1,
      "start_pref_fm":      5000,
      "limit_pref_fm":      5,
      "start_pref_v":       0,
      "limit_pref_v":       0
  },
```
where `start_pref_e`, `limit_pref_e`, `start_pref_fr`, `limit_pref_fr`, `start_pref_fm`, `limit_pref_fm`, `start_pref_v` and `limit_pref_v` determines the starting and ending weight of energy, atomic forces, magnetic forces and virial respectively. Here `start_pref_v` and `limit_pref_v` are set to 0 for the lack of virial data.

### Train a model
The training of a DeepSPIN model for NiO can be simply executed by running
```
$ cd example/NiO/train
$ dp train NiO_spin.json
```

If the training process is successfully running, a series of files can be found in the current directory. The learning curve can be viewed from `lcurve.out`. More details can refer to the [documentation](https://github.com/deepmodeling/deepmd-kit/blob/master/doc/train/training.md).

### Freeze a model
When the training is finished, The DeepSPIN model can be extracted from a checkpoint and dumped into a protobuf file `*.pb`. This process is called "freezing" by running
```
$ dp freeze -o model.pb
```
then the frozen DeepSPIN model is obtained as `model.pb`.

### Test a model
The frozen DeepSPIN model can then be used in many ways. The most straightforward test can be performed as following
```
$ dp test -m model.pb -s ../data/validation -n 20 -d detail
```
where `-m` gives the frozen model, `-s` gives the path to the tested system, `-n` gives the number of tested frames and `-d` gives the detailed information about the tested system. 

If the testing is successfully performed, a series of files can be found in the current directory, including `deatil.*.out`. The constrast between predicted values and labelled values of the energy, atomic forces and magnetic forces can be found in `deatil.e.out`, `deatil.fr.out` and `deatil.fm.out` respectively.







