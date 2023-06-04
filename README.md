# DeepSPIN

---------------------------------------------------------------------

DeepSPIN is a method of constructing a deep learning potential for magnetic systems described in the paper [*Screening Spin Lattice Interaction Using Deep Learning Approach*](https://doi.org/10.48550/arXiv.2304.09606). DeepSPIN has been integrated into [DeePMD-kit](https://github.com/deepmodeling/deepmd-kit) as a new feature of the release version `v2.2.2`. The files and code snippets involved in the development of DeepSPIN are detailed in this [Pull Request](https://github.com/deepmodeling/deepmd-kit/pull/2304/files).

## Contents

- [How to cite](#how-to-cite)
- [Requirements](#requirements)
- [Demo: DeepSPIN model of NiO](#demo-deepspin-model-of-nio)
    - [Dataset preparation](#dataset-preparation)
    - [Input script preparation](#input-script-preparation)
    - [Train a model](#train-a-model)
    - [Freeze a model](#freeze-a-model)
    - [Test a model](#test-a-model)
    - [Typical running time](#typical-running-time) 
- [Demo: Energy minimization](#demo-energy-minimization)
    - [Install LAMMPS](#install-lammps)
    - [Run the minimization](#run-the-minimization)

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

To utilize the DeepSPIN method, Python 3.7 or later environments and following packages are required：

- deepmd-kit=2.2.2
- dpdata

In Linux, you can easily achieve the requirements and install DeepSPIN with `conda`,

```shell
# install miniconda with python 3.9
wget https://repo.anaconda.com/miniconda/Miniconda3-py39_4.10.3-Linux-x86_64.sh
bash Miniconda3-py39_4.10.3-Linux-x86_64.sh

# install deepmd-kit packages with conda 
conda create -n deepmd deepmd-kit=2.2.2 -c conda-forge

# enable the environment
conda activate deepmd

# install dpdata packages with pip
pip install dpdata
```

Note that you can contain the `CPU` version of DeepSPIN by above commands, if you want to obtain the `GPU` version, please follow the [guide](https://github.com/deepmodeling/deepmd-kit/blob/master/doc/install/easy-install.md#with-conda).

## Demo: DeepSPIN model of NiO

The usage of DeepSPIN is similar to that of [DeePMD-kit](https://github.com/deepmodeling/deepmd-kit). The guide of a quick start on DeePMD-kit can be found [here](https://github.com/deepmodeling/deepmd-kit/blob/master/doc/getting-started/quick_start.ipynb). In this part we show an example of how to obtain a DeepSPIN model of antiferromagnetic NiO system. 

### Dataset preparation

The original training data should be sourced from the first-principle non-collinear magnetic excitation calculation method [DeltaSpin](https://github.com/caizefeng/DeltaSpin) based on VASP, in which the lattice configuration is collected from `POSCAR`, the atomistic spin configuration is collected from `INCAR`, the potential energy of system and atomic forces are collected from `OUTCAR`, and magnetic forces are collected from `OSZICAR`. We take the 32-atom $2\times2\times2$ NiO rhombohedral supercell for the calculation and collect DFT results. 

After some preprocessing, the format of original data should be converted to fit the general input file requirements for DeePMD-kit, as shown in `example/NiO/raw`, more detailed description is given in this [documentation](https://github.com/deepmodeling/deepmd-kit/blob/master/doc/data/index.md). It's worth noting that the type of pseudo atoms around Ni atoms is represented by `2` in `type.raw`, the Cartesian coordinates of pseudo atoms are stitched after the coordinates of real atoms sequentially by index in `coord.raw`, and the magnetic forces of Ni atoms are stitched after the atomic forces of real atoms sequentially as well in `force.raw`. 

Then the data should be split into training set and validation set, converted from `.raw` format to `.npy` format using the script `raw_to_set.sh` in `example/NiO/scripts`. For example, we provide 100 frames of data in `example/NiO/raw`,

```shell
cd example/NiO/raw
sh ../scripts/raw_to_set.sh 20
```

It will generate 5 subsets from `set.000` to `set.004` in the current directory, with each subset containing 20 frames. The former 4 subsets can be picked as the training set, while the last one will be the validation set, as shown in `example/NiO/data`. 

### Input script preparation

Once the dataset preparation is complete, a `json` format input script is required to specify the parameters for model training. Here we take `example/NiO/NiO_spin.json` as an example, and the parameters dedicated to DeepSPIN are introduced. For detailed information about other parameters, please refer to this [documentation](https://github.com/deepmodeling/deepmd-kit/tree/master/doc/model).

The hyper-parameters dedicated to DeepSPIN is set in the following section

```json
    "spin" : {
        "use_spin":         [true, false],
        "virtual_len":      [0.4],
        "spin_norm":        [1.2737]
    },
```
* `use_spin` determines whether the atom type is magnetic. Here We set `true` for Ni and `false` for O.
* `virtual_len` specifies the distance between the pseudo atom and its corresponding real atom. Here we set 0.4 $\text{\AA}$ for Ni.
* `spin_norm` specifies the magnitude of the magnetic moment for each magnatic atom. Here we set 1.2737 $\mu_B$ for Ni.

The loss function for the DeepSPIN model is set in the following section

```json
    "loss" : {
        "type":               "ener_spin",
        "start_pref_e":       0.02,
        "limit_pref_e":       10,
        "start_pref_fr":      1000,
        "limit_pref_fr":      1,
        "start_pref_fm":      5000,
        "limit_pref_fm":      5,
        "start_pref_v":       0,
        "limit_pref_v":       0
    },
```

where `start_pref_e`, `limit_pref_e`, `start_pref_fr`, `limit_pref_fr`, `start_pref_fm`, `limit_pref_fm`, `start_pref_v` and `limit_pref_v` determines the starting and ending weight of energy, atomic forces, magnetic forces and virial in the loss function respectively. Here `start_pref_v` and `limit_pref_v` are set to 0 for the lack of virial data in the dataset.

### Train a model

The training of a model can be simply executed by running

```shell
cd example/NiO/train
dp train NiO_spin.json
```

If the training process is successfully running, a series of files will be generated in the current directory. The learning curve on both training set and validation set can be viewed from `lcurve.out`. For more details please refer to this [documentation](https://github.com/deepmodeling/deepmd-kit/blob/master/doc/train/training.md).

### Freeze a model

When the training is finished, the DeepSPIN model can be extracted from a checkpoint and dumped into a protobuf file `*.pb`. This process is called "freezing" by running

```shell
dp freeze -o model.pb
```

Then you will obtain the frozen DeepSPIN model `model.pb`.

### Test a model

The frozen DeepSPIN model can then be used in many ways. The most straightforward test can be performed as follows,

```shell
dp test -m model.pb -s ../data/validation -n 20 -d detail
```

where `-m` gives the tested model, `-s` gives the path to the tested system, `-n` gives the number of tested frames and `-d` gives the detailed predictions about the tested system. 

If the testing is successfully performed, a series of files will be generated in the current directory, including `deatil.*.out`. The contrast between predicted values and label values of the energy, atomic forces and magnetic forces can be viewed in `deatil.e.out`, `deatil.fr.out` and `deatil.fm.out` respectively.

### Typical running time

The above sessions are tested on the 8-core 16G-RAM x86_64 CPU machine, with the CPU model name being `Inter(R) Xeon(R) Platinum 8163 CPU @ 2.50GHz`. The average elapsed time for each session is listed below,

|  session  | running time (s) |
|  :-----:  |     :-----:      |
| dp train  |      71.14       |
| dp freeze |      5.02        |
| dp test   |      4.74        |

## Demo: Energy minimization

The trained DeepSPIN model can be applied in the molecular dynamics simulation software [LAMMPS](https://github.com/lammps/lammps/tree/develop) for energy minimization and spin-lattice dynamics simulations of magnetic systems. In this part we show an example of how to simulate the energy minimization in the NiO grain boundary system using a DeepSPIN model.

### Install LAMMPS 

Unlike the previous demo, you may manually install DeePMD-kit from source codes by following the instructions [installing the Python interface](https://github.com/deepmodeling/deepmd-kit/blob/master/doc/install/install-from-source.md#install-the-python-interface) and [installing the C++ interface](https://github.com/deepmodeling/deepmd-kit/blob/master/doc/install/install-from-source.md#install-the-c-interface), since the C++ interface is necessary when using DeePMD-kit together with LAMMPS. Then you may make the DeePMD-kit module for LAMMPS.

```shell
cd $deepmd_source_dir/source/build
make lammps
```

It will generate a module called `USER-DEEPMD` in the `build` directory.

In order to utilize the DeepSPIN model, the source codes of LAMMPS need some modifications. Here we modified the source codes based on the version `stable_23Jun2022_update1`, as shown in `lammps_DeepSPIN`. You may download it, copy the module `USER-DEEPMD`, and compile necessary modules as follows,

```shell
cd lammps_DeepSPIN/src
cp -r $deepmd_source_dir/source/build/USER-DEEPMD .
make yes-kspace
make yes-manybody
make yes-spin
make yes-user-deepmd
```

Then build LAMMPS and end up with an executable `lmp_mpi` as follows,

```shell
make mpi -j 4
```

### Run the minimization

We provide the input and output files of running the energy minimization in `example/minimization`. `init.data` specifies the initial lattice and spin configuration of NiO grain boundary system with 11520 atoms. For the detailed introduction of data file format please refer to this [documentation](https://docs.lammps.org/read_data.html). `model.pb` is the trained DeepSPIN model. `minimize.in` is the input script to run LAMMPS. You can simply run the minimization by mpirun,

```shell
mpirun -np 1 lammps_DeepSPIN/src/lmp_mpi -in minimize.in
```

You can change `-np` to the number of processes suitable to your own machine.

When the minimization is finished, a series of output files will be generated in the current directory. Especially, `dump.out` records the optimized spin configuration of the system, in which you may see a deviation of the spin configuration from the AFM ground state near the grain boundary.