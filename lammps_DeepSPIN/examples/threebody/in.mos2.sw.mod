# monolayer MoS2
units      	metal
boundary   	p p f
processors	* * 1

atom_style 	atomic
read_data       single_layer_MoS2.data

mass            * 32.065	# mass of sulphur atom , uint: a.u.=1.66X10^(-27)kg
mass            1 95.94		# mass of molebdenum atom , uint: a.u.=1.66X10^(-27)kg

########################## Define potentials ################################
pair_style      sw/mod maxdelcs 0.25 0.35
pair_coeff      * * tmd.sw.mod Mo S S
#########################################################################

### Simulation settings ####
timestep        0.001
velocity        all create 300.0 12345

############################

# Output
thermo          500
thermo_style    custom step etotal pe ke temp
thermo_modify   lost warn

###### Run molecular dynamics ######
fix             thermostat all nve
run             5000
