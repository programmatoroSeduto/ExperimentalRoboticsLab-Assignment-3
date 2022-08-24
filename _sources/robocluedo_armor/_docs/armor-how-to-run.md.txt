
# DOCS -- how to run robocluedo armor

---

```{toctree}
---
caption: Contents
maxdepth: 2
---
./armor-how-to-run.md
```

---

## Ontology paths configuration

the package contains a file called *parameters.launch* inside the *config* folder.

from there, you can set

- cluedo_path_owlfile : the path where the base ontology is located
- cluedo_path_owlfile_backup : the path where to export the actual ontology

## launch file

in order to run the package, my advice is to use the launch file `run.launch` inside the package:

```bash
roslaunch robocluedo_armor run.launch
```

it runs two compoennts:

- aRMOR
- and the node implementing the services of the interface

### expected output

you should see a output which is similar to this one:

```text
... logging to /root/.ros/log/fdcd8fc4-1c7f-11ed-a0ec-0242ac110002/roslaunch-3b17871017fd-3138.log
Checking log directory for disk usage. This may take a while.
Press Ctrl-C to interrupt
Done checking log file disk usage. Usage is <1GB.

started roslaunch server http://3b17871017fd:39871/

SUMMARY
========

PARAMETERS
 * /cluedo_path_owlfile: /root/ros_ws/src/...
 * /cluedo_path_owlfile_backup: /root/ros_ws/src/...
 * /rosdistro: noetic
 * /rosversion: 1.15.14

NODES
  /
    armor_service (armor/execute)
    robocluedo_armor_interface (robocluedo_armor/robocluedo_armor_interface)

auto-starting new master
process[master]: started with pid [3176]
ROS_MASTER_URI=http://localhost:11311

setting /run_id to fdcd8fc4-1c7f-11ed-a0ec-0242ac110002
process[rosout-1]: started with pid [3201]
started core service [/rosout]
process[armor_service-2]: started with pid [3204]
process[robocluedo_armor_interface-3]: started with pid [3212]
[ INFO] [1660557157.044513400]: [cluedo_armor_interface] Ontology found! [/root/ros_ws/src/erl2-new/robocluedo_armor/config/cluedo_owl_ontology/cluedo_ontology.owl] 
[ INFO] [1660557157.045186400]: [cluedo_armor_interface] loading armor ...
[ INFO] [1660557157.045285200]: [armor_tools] Requiring client [/armor_interface_srv] ...
[ INFO] [1660557157.045629100]: waitForService: Service [/armor_interface_srv] has not been advertised, waiting...
Loading node class: it.emarolab.armor.ARMORMainService
log4j:WARN No appenders could be found for logger (org.ros.internal.node.client.Registrar).
log4j:WARN Please initialize the log4j system properly.
log4j:WARN See http://logging.apache.org/log4j/1.2/faq.html#noconfig for more info.
[ INFO] [1660557157.457678500]: waitForService: Service [/armor_interface_srv] is now available.
[ INFO] [1660557157.457737900]: [armor_tools] -> OK
 15/08/22_09:52:37,645 -> Class OWLReferencesInterface$OWLReferencesContainer : Create a new prefix manager for References: OWLReferencesInterface "cluedo"
 15/08/22_09:52:38,058 -> Class OWLReferencesInterface$OWLReferencesContainer : Ontology OWL manager created for References: OWLReferencesInterface "cluedo"
 15/08/22_09:52:38,200 -> Class OWLReferencesInterface$OWLReferencesContainer : Ontology loaded from file  for References: OWLReferencesInterface "cluedo"
 15/08/22_09:52:38,200 -> Class OWLReferencesInterface$OWLReferencesContainer : Create a OWL Data Factory for References: OWLReferencesInterface "cluedo"
 15/08/22_09:52:38,273 -> Class OWLReferencesInterface$OWLReferencesContainer : Reasoner (com.clarkparsia.pellet.owlapi.PelletReasonerFactory) created in: 73000900 [ns] for the References: OWLReferencesInterface "cluedo"
 15/08/22_09:52:38,282 -> OWLReferences OWLReferencesInterface "cluedo" : new OWL References initialised in: 780012400 [ns] for the Object OWLReferencesInterface "cluedo"
[ INFO] [1660557158.369155800]: [cluedo_armor_interface] OK!
[ INFO] [1660557158.369224200]: [cluedo_armor_interface] opening server [/cluedo_armor/add_hint]  ...
[ INFO] [1660557158.369796400]: [cluedo_armor_interface] OK!
[ INFO] [1660557158.369854300]: [cluedo_armor_interface] opening server [/cluedo_armor/find_consistent_h]  ...
[ INFO] [1660557158.370445800]: [cluedo_armor_interface] OK!
[ INFO] [1660557158.370497300]: [cluedo_armor_interface] opening server [/cluedo_armor/wrong_hypothesis]  ...
[ INFO] [1660557158.370983600]: [cluedo_armor_interface] OK!
[ INFO] [1660557158.371037500]: [cluedo_armor_interface] opening server [/cluedo_armor/backup]  ...
[ INFO] [1660557158.371432900]: [cluedo_armor_interface] OK!
[ INFO] [1660557158.371482100]: [cluedo_armor_interface] ready!

```

here's also the output from `rosservice list` command:

```text
root@3b17871017fd:~/ros_ws/src/erl2-new# rosservice list
/armor_interface_serialized_srv
/armor_interface_srv
/cluedo_armor/add_hint
/cluedo_armor/backup
/cluedo_armor/find_consistent_h
/cluedo_armor/wrong_hypothesis
/robocluedo_armor_interface/get_loggers
/robocluedo_armor_interface/set_logger_level
/rosout/get_loggers
/rosout/set_logger_level

```

ready to use!
