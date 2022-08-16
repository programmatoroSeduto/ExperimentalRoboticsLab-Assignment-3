
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

## 16/08/2022 -- 

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
