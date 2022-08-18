
# RoboCLuedo -- erl3 -- Development Log

---

```{toctree}
---
caption: Indice 
---
./dev-log.md
```

---

- [plantUML online](http://www.plantuml.com/plantuml/uml/VLNlQkGs4F-kfvWB77NWuetthd-Q3-NI59h0fLt8XdufB8eqNelOaf7acAKK-XfzlJv9PoHxDybo-M2FD7z-C_ERyUxd4AMFGzSAyKvZRIo22t952cXYxCF5Ok7bM6vDR8Q78Q1NpaQqiLIkMrnv6HhKdR5wiMgbZVUtNyvSZpQW6ho9E-bLOoAgE7XSdXcA3OjEXeXUl3DMjOFUfrjSkQvpjkpfV6oyfymBsRPVCLzBhqVfyGsNMnFK6-Oxz4zlfhWpyHcy-AQ4M-btO098-0MViAM-FHWBiK5OUQS75I6Yx4gu8qqZsV4FOigD0QfpM5s1j9eUkBJQEwEXRvp5af5_TWyP-5PQkJt0NYhb1Xl3X7kTOCb9pL1cjSUuUU9xOEtD6hR33iR6GUlS8-dgY3uXXjHs2HonRd07D2ABNBbBTejnTFuHQFWVKf8d8q52RJoEHCRiTcC9a7nBGSm03ok8wBP8DWz_2U9mmxkpsNf4kz4pNGLJ-05EM9oGV4uRt_Uydfo-nc2jZCRPK72dvCo2WwZRzHIVXmgNlA774BJEnc88cowpN13j54HlZdt1DIkcMH3EtzmarMOK7hMfCJn6rnUzef3gnsLPVVT3MPNLEKCSnf-wlPfgQcNF8Pry5RFCQTKiidUUSM5w5apIpihEABXPiL-sGbNn9PrVXicyR4Tnqn9IQQy3yueKMRngAZdVFn1F0qnaBy_Byq_mPNrt642cZ3ZxBM-Jc9XY0ZUZyTZo5BmR8kKPJMqkLo_pCDGKEnL5-rZGG_hVwvfWW2xiTKqYUymhMim7idvRbH-_BvUVgFAFnvYgTOCkqfjiyqg_z1CYVVLdPpz1houWkC5JSkKq7WmJnMnLHhRGeJPI2DghPEua1TR6IfkinjPkRyj3lO0aG57LWLBoPYTpdi45VwIrsGxv0n0YHoMFp9wOIdYkoe8rp9KG6Mj_fwhsv_vm58BUwJRJACHyIkf45-cSYuJslZOjqbfXSGeUhKQFcWg83Sn_3q190rKDxwT3SVgdNJS8roQgfZ6FwF_xvzy0lmv68qIt3nIV2I_zz0hnFm00)
- [un altro editor motlo carino per PlantText](https://www.planttext.com/)
- [onlineGDB](https://www.onlinegdb.com/online_c++_compiler)

## 15/08/2022 -- setup repository -- starter kit -- 

nuovo assignment, nuova avventura:

- primo setup della repository
- **COMMIT**: "simil-fork from erl2"
- creazione di un nuovo log pulito (serve a me per riflettere e annotare cose che non so dove metterei altrimenti)
- customizzazione della documentazione (... il 2 diventa un 3?)
- **COMMIT**: "setup (docs)"

---

- importazione materiale del prof per il terzo assignment
- [link starter kit per l'assignment](https://github.com/CarmineD8/exp_assignment3/)
- modalità di sviluppo al nuovo giocattolo
	- ANZI, per il momento niente modalità di sviluppo
	- più avanti semmai ne inserirò una
- compila? compila (perfetto)
- e senza perdere tempo, voglio una documentazione per il nuovo nodo
	- uml
	- aggiungo anche una minima copde reference
	
- **potrei avere qualche problema col nome dei services più avanti** perchè purtroppo adesso */oracle_hint* è un servizio, e non più un topic. servirà un remapping ... o forse posso semplicemente cambiare nome del servizio nel mission manager, o fare un remapping da launch file
- **COMMIT**: "setup (starter kit with oracle documentation)"

---

prima di tutto serve avere funzionante il modello del robot. La cosa più ragionevole in questo momento è partire dall'installazione di MoveBasepassando per il modello del robot. Questo terzo assignment utilizzerà una versioneleggermente diversa del robot, equipaggiata con 3 telecamere e con MoveBase integrato. *partiamo dal modello e passiamo da move_base*; alle telecamere ci pensiamo dopo. 

- creazione del nuovo modello URDF
- e creazione del nuovo maledettissimo pacchetto MoveIt (sperando di non combinare casini stavolta)
- la nuova versione del robot avrà come soprannome "hunter"
- adesso tutti i noiosissimi setup (pensa se il setup assistant funzionasse davvero ... sarebbe un mondo magnifico no? e invece siamo qui)
	- fix del world file
	- "run"  launch file (altro fix da segnare)
	- non avevo fixato RViz ... dovrò farlo prima o poi
- direi che il pacchetto è pronto!

- ...hmmm però aspetta: e che dire del *mastino di Baskerville?* che in inglese è *the HOUD of baskerville?* chiamarlo Hunter sarebbe ... un'occasione mancata
	- e quindi, cambiamo tutti i nomi? *certo che no.*

- **COMMIT**: "working on robot model (robocluedo HUNTER)"
- *e ci si rivede domani*

---

## 16/08/2022 -- modello del robot

cotinuiamo a lavorare sul modello del robot (oggi si fa poco...)

- il mio robot avrà ben 4 telecamere (giusto per complicare un po' la situazione, ecco)
	- una frontale *bassa*
	- una sul cluedo link
	- un'altra a destra del robot
	- e un'altra a sinistra, un po' come Spot
- prima il codice per applicare le videocamere

parte Gazebo URDF:

```xml
<!-- camera -->
<gazebo reference="camera_link">
<sensor type="camera" name="camera1">
  <update_rate>30.0</update_rate>
  <camera name="head">
	<horizontal_fov>1.3962634</horizontal_fov>
	<image>
	  <width>800</width>
	  <height>800</height>
	  <format>R8G8B8</format>
	</image>
	<clip>
	  <near>0.02</near>
	  <far>300</far>
	</clip>
	<noise>
	  <type>gaussian</type>
	  <mean>0.0</mean>
	  <stddev>0.007</stddev>
	</noise>
  </camera>
  <plugin name="camera_controller" filename="libgazebo_ros_camera.so">
	<alwaysOn>true</alwaysOn>
	<updateRate>0.0</updateRate>
	<cameraName>robot/camera1</cameraName>
	<imageTopicName>image_raw</imageTopicName>
	<cameraInfoTopicName>camera_info</cameraInfoTopicName>
	<frameName>camera_link</frameName>
	<hackBaseline>0.07</hackBaseline>
	<distortionK1>0.0</distortionK1>
	<distortionK2>0.0</distortionK2>
	<distortionK3>0.0</distortionK3>
	<distortionT1>0.0</distortionT1>
	<distortionT2>0.0</distortionT2>
  </plugin>
</sensor>
</gazebo> 
```

Dovrò aggiungere dei link per farci stare le videocamere...

- e andiamo a modificare il modello del robot
- la camera davanti bassa
	- creazione dei link nel modello URDF
	- compila? compila
	- e creazione della videocamera montata sul link
	- e ora vediamo se funge ... e pare che funga
- voglio rivedere meglio la questione dei topic della videocamera

```text
/robot/camera_front_low/camera_info
/robot/camera_front_low/image_raw
/robot/camera_front_low/image_raw/compressed
/robot/camera_front_low/image_raw/compressed/parameter_descriptions
/robot/camera_front_low/image_raw/compressed/parameter_updates
/robot/camera_front_low/image_raw/compressedDepth
/robot/camera_front_low/image_raw/compressedDepth/parameter_descriptions
/robot/camera_front_low/image_raw/compressedDepth/parameter_updates
/robot/camera_front_low/image_raw/theora
/robot/camera_front_low/image_raw/theora/parameter_descriptions
/robot/camera_front_low/image_raw/theora/parameter_updates
/robot/camera_front_low/parameter_descriptions
/robot/camera_front_low/parameter_updates
```

- il sistema funziona! Ora aggiungiamo quella sul braccio
	- compilare compila
	- e prova con rviz gazebo ... ruotata nel verso sbagliat! ... e ok
	- telecamera laterale 1 ... ok
	- e telecamera laterale 2 ... ok
	- genera? genera
	- e in RViz pare funzioni tutto come dovrebbe
- **COMMIT**: "working on robot model (added cameras)"

---

e adesso, installazione di ArUco e openCV

- prima di tutto, voglio avere delle versioni mie di OpenCV installate sulla macchina, quella effettivamente data per l'assignment (sarà un errore? in caso ho il backup della ws intera)
- installazione di openCV per ROS
	- compila? compila.
- meglio scriversi un piccolo documento dove spiego come installare queste cose
- importazione dei modelli arUco
- compilazione ... compila
- e adesso proviamo se i markers funzionano
	- **funge!**
	- meglio copiare tutt i markers anche nel package worlds
- **COMMIT**: "setting up openCV and ArUco"

---

adesso che ArUco c'è, possiamo iniziare il lavoro serio

- questo nuovo progetto richiede ... un **NUOVO PACKAGE**

```bash
catkin_create_pkg robocluedo_vision roscpp rospy std_msgs geometry_msgs nav_msgs sensor_msgs erl2 exp_assignment3 aruco aruco_msgs dynamic_reconfigure cv_bridge image_transport 

```

- eeee documentazione, subito
- **COMMIT**: "working on vision (setup package)"

--- 

## 17/08/2022 -- move_base navigation

iniziamo subito da move_base, per passare poi al navigation system per il nuovo assignment. ci sarà da rivedere anche il manipulation system, dato che le postures del robot sono cambiate. (magari documentare le postures?)


- installazione di move_base (dalla vecchia versione di erl2)
- devo anche fare una modifica a come viene lanciato RViz in modo da poter scegliere il file di configurazione
- ora dovrebbe esserci tutto: posso provare il navigation stack ... *e funziona a primo colpo signori*
- meglio annotarsi due cose nella documentazione del robot riguardo il navigation stack
	- (mi ringrazierò in futuro, quando dovrò installare di nuovo move_base su un altro robot)
- ultima prova, giusto per capire se è tutto a posto
- **COMMIT**: "working on robot model (setup move base)"

--- 

ora, il navigation controller del robot

- anzitutto, il nodo che deve eseguire la navigation: penso avrà lo stesso meccanismo dei nodi di bug_m : trigger, e wait del segnale, e struttura come macchina a stati
- **nuovo nodo** : move_base_nav.py
	- prima struttura del nodo
	- (c'è voluto un po' più del previsto...)
- prima di andare avanti, voglio vedere se il nodo si avvia e non crasha

```bash
# shell 1
roslaunch robocluedo_robot_hunter run.launch 2>/dev/null

# shell 2
rosparam set des_pos_x 0.0
rosparam set des_pos_y 0.0
rosparam set des_yaw 0.0
rosrun robocluedo_movement_controller move_base_nav.py

# shell 3
rosservice call /nav_stack_go_to_point_switch "data: true" 
rosservice call /nav_stack_go_to_point_switch "data: false" 

```

- e ora torniamo ad implementare questo bellissimo nodo
- **COMMIT**: "workign on nvigation (implementation of the new navigation service)"

---

e torniamo al lavoro sul navigation system

- occorre aggiungere la "action" move_base...
	- ...che tratterò come un topic per il momento, *perchè sono pigro*
	- recupero la documentazione su move_base dal vecchiop assignment
	- ... e ovviamnete manca la parte su come inviare la richiesta di cancellazione tramite topic, dannato me
- **ALT FERMI TUTTI** c'è un grosso problema con move_base ... al solito
	- il robot continua ad indietreggiare quando move_base è attivo
	- la traiettoria calcolata parte *dal retro del robot* ... eppure go to point ha sempre funzionato, quindi perchè move_base da il comando invertito? *forse un problema col modello urdf?* 
	- che genere di twist viene inviato da move_base? 
		
		```text
		linear: 
		  x: -0.1
		  y: 0.0
		  z: 0.0
		angular: 
		  x: 0.0
		  y: 0.0
		  z: 0.0
		```
	
	- non è un problema del modello: è esattamente identico a quello del precedente assignment (erl2 quello vecchio), funzionava, deve continuare a funzionare
	- ... se provassi a cambiare il controller?
	- potrebbe esserci un problema col local planner
	- credo che il problema possa essere più stupido di quel che sembra: il robot non riesce a curvare per via delle ruote dietro ... l'altro nodo ignorava questo aspetto dinamico, *ma a quanto pare a move_base non sfugge proprio nulla.* ci fosse un modo per diminuire l'attrito delle ruote dietro e renderle così indifferenti alla rotazione...
- il problema è più fastidioso di quel che sembra: nemmeno annullando l'attrito si riesce. il local planner nella visualizzazione RViz mostra un buco nero davanti al robot, segno che ha individuato un ostacolo, perciò indietreggia... il problema è che non c'è alcun ostacolo!
	- il problema potrebbe essere nel laucnher? ha senso? 
		- l'unica cosa regolabile da launch è GMapping, quindi no
	- allora il problema è in qualche configuration file...
	- prima però voglio provare a muovere il robot inviando direttamente un comando su cmd_vel e vedere che succede...
		- muoversi si muove...
- ... e mi viene un altro dubbio:
	- i sensori laser vanno ad incidere sulle ruote di fianco ... questo in effetti potrebbe portare MoveBase a vedere degli ostacoli attorno al robot, e preferire così un certo tipo di movimento per evitare l'ostacolo
	- la soluzione potrebbe quindi essere spostare il sensore laser in avanti...
	- provo ... 
	- ora che ho modificato il modello, passo a provare con move base ... **E PROBLEMA RISOLTO SIGNORI**: ora va dove voglio io!
- **COMMIT**: "working on robot model (move_base laser issue)"
- e andiamo avanti con lo sviluppo del nodo
	- quindi ... questa cancellazione?
	- e torniamo ad implementare
	- (oggi sto facendo una fatica incredibile...)
- è arrivata la parte migliore: *testare...*

```bash
# shell 1
roslaunch robocluedo_robot_hunter run.launch world_name:=square_room.world 2>/dev/null

# shell 2
rosparam set des_pos_x 0.0
rosparam set des_pos_y 0.0
rosparam set des_yaw 0.0
rosrun robocluedo_movement_controller head_orientation.py

# shell 3
rosrun robocluedo_movement_controller move_base_nav.py

# shell 4
rosparam set des_pos_x 2.0
rosparam set des_pos_y 0.0
rosparam set des_yaw 0.0
rosservice call /nav_stack_go_to_point_switch "data: true" 
sleep 30
rosservice call /nav_stack_go_to_point_switch "data: false" 

rosparam set des_pos_x 0.0
rosparam set des_pos_y -1.5
rosparam set des_yaw 0.0
rosservice call /nav_stack_go_to_point_switch "data: true" 
sleep 30
rosservice call /nav_stack_go_to_point_switch "data: false" 

```

- le cose ora iniziano a funzionare... ultimo test:

```bash
# shell 1
roslaunch robocluedo_robot_hunter run.launch world_name:=square_room.world 2>/dev/null

# shell 2
rosparam set des_pos_x 0.0
rosparam set des_pos_y 0.0
rosparam set des_yaw 0.0
rosrun robocluedo_movement_controller head_orientation.py &
rosrun robocluedo_movement_controller move_base_nav.py

# shell 3
rosparam set des_pos_x 2.0
rosparam set des_pos_y 0.0
rosparam set des_yaw 0.0
rosservice call /nav_stack_go_to_point_switch "data: true" 
sleep 30
rosservice call /nav_stack_go_to_point_switch "data: false" 

rosparam set des_pos_x 0.0
rosparam set des_pos_y -1.5
rosparam set des_yaw 0.0
rosservice call /nav_stack_go_to_point_switch "data: true" 
sleep 30
rosservice call /nav_stack_go_to_point_switch "data: false" 

rosparam set des_pos_x 0.0
rosparam set des_pos_y 2.0
rosparam set des_yaw 0.0
rosservice call /nav_stack_go_to_point_switch "data: true" 
sleep 30
rosservice call /nav_stack_go_to_point_switch "data: false" 

rosparam set des_pos_x -2.0
rosparam set des_pos_y 0.0
rosparam set des_yaw 0.0
rosservice call /nav_stack_go_to_point_switch "data: true" 
sleep 30
rosservice call /nav_stack_go_to_point_switch "data: false" 

```

- *e funge.*
- **COMMIT**: "working on navigation (move_base navigation node tested, ready to use)"

---

e ora il controller, altro dolore:

- implementazione del controller (molto simile a quello di bug_m)
- compila? alla prima senza errori, sono un campione
- e ora, prova pratica

```bash
# shell 1
roslaunch robocluedo_robot_hunter run.launch world_name:=square_room.world 2>/dev/null

# shell 2
roslaunch robocluedo_movement_controller navigation_system.launch

# shell 3
rosrun robocluedo_movement_controller navigation_manager

# shell 4
rosservice call /navigation_manager/set_algorithm "algorithm: 1
enabled: false"

rosservice call /navigation_manager/navigation "target:
  x: 1.5
  y: 2.0
  yaw: 1.0
force_enable: false" 

# this sequence moves the robot
rosservice call /navigation_manager/set_algorithm "algorithm: 1
enabled: true"

rosservice call /navigation_manager/navigation "target:
  x: -1.5
  y: 2.0
  yaw: 1.0
force_enable: false" 

# this sequence moves the robot EVEN IF the component has been turned off
rosservice call /navigation_manager/set_algorithm "algorithm: 1
enabled: false"

rosservice call /navigation_manager/navigation "target:
  x: 1.5
  y: -2.0
  yaw: 1.0
force_enable: true" 

```

- FUNZIONAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
- **COMMIT**: "working on navigation (controller implemented and tested, ready to use)"

---

## 18/08/2022

oggi, manipulation controller

- anzitutto, documentazione riguardo le poses del robot
- nel manipulation controller bisogna solo cambiare i codici e i nomi delle postures, tutto qui
	- già solo cambiando i nomi delle costanti il lavoro è finito
	- compila? e compila

giusto un  piccolo test per capire se ho scritto bene i nomi delle costanti (impossibile escludere qualche errore stupido)

```bash
# shell 1
roslaunch robocluedo_robot_hunter run.launch world_name:=square_room.world 2>/dev/null

# shell 2
rosrun robocluedo_movement_controller manipulation_controller 2>/dev/null

# shell 3
rosservice call /tip_pos "command: 0" 
sleep 2
rosservice call /tip_pos "command: 1" 
sleep 2
rosservice call /tip_pos "command: 2" 
sleep 2
rosservice call /tip_pos "command: 3" 
sleep 2
rosservice call /tip_pos "command: 4" 
sleep 2
rosservice call /tip_pos "command: 5" 
sleep 2
rosservice call /tip_pos "command: 6" 
sleep 2
rosservice call /tip_pos "command: 7" 
sleep 2
rosservice call /tip_pos "command: 8" 
sleep 2

# UNKNOWN POSE
rosservice call /tip_pos "command: 9" 
sleep 2

# UNKNOWN POSE
rosservice call /tip_pos "command: -1" 
sleep 2
```

- funzionare funziona
- **COMMIT**: "working on manipulation (new postures)"


- .... ma devo ammettere che non mi piace molto come il robot fa il planning




## Note

### utilizzo di move base

- il nodo che si occupa di fare la navigation aggiorna il plan parecchie volte, perchè sulle lunghe tratte move_base tende a far andare il robot in percorsi che non hanno molto senso
- rifacendo il panning ogni tot si assicura una certa sitabilità sul path prescelto
- unico difetto: il robot si ferma spesso per fare replanning, causando delle oscillazioni in avanti un po' brusche
