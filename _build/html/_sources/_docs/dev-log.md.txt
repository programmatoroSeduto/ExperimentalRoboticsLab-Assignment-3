
# RoboCLuedo -- Development Log

```{toctree}
./dev-log.md
```

- [plantUML online](http://www.plantuml.com/plantuml/uml/VLNlQkGs4F-kfvWB77NWuetthd-Q3-NI59h0fLt8XdufB8eqNelOaf7acAKK-XfzlJv9PoHxDybo-M2FD7z-C_ERyUxd4AMFGzSAyKvZRIo22t952cXYxCF5Ok7bM6vDR8Q78Q1NpaQqiLIkMrnv6HhKdR5wiMgbZVUtNyvSZpQW6ho9E-bLOoAgE7XSdXcA3OjEXeXUl3DMjOFUfrjSkQvpjkpfV6oyfymBsRPVCLzBhqVfyGsNMnFK6-Oxz4zlfhWpyHcy-AQ4M-btO098-0MViAM-FHWBiK5OUQS75I6Yx4gu8qqZsV4FOigD0QfpM5s1j9eUkBJQEwEXRvp5af5_TWyP-5PQkJt0NYhb1Xl3X7kTOCb9pL1cjSUuUU9xOEtD6hR33iR6GUlS8-dgY3uXXjHs2HonRd07D2ABNBbBTejnTFuHQFWVKf8d8q52RJoEHCRiTcC9a7nBGSm03ok8wBP8DWz_2U9mmxkpsNf4kz4pNGLJ-05EM9oGV4uRt_Uydfo-nc2jZCRPK72dvCo2WwZRzHIVXmgNlA774BJEnc88cowpN13j54HlZdt1DIkcMH3EtzmarMOK7hMfCJn6rnUzef3gnsLPVVT3MPNLEKCSnf-wlPfgQcNF8Pry5RFCQTKiidUUSM5w5apIpihEABXPiL-sGbNn9PrVXicyR4Tnqn9IQQy3yueKMRngAZdVFn1F0qnaBy_Byq_mPNrt642cZ3ZxBM-Jc9XY0ZUZyTZo5BmR8kKPJMqkLo_pCDGKEnL5-rZGG_hVwvfWW2xiTKqYUymhMim7idvRbH-_BvUVgFAFnvYgTOCkqfjiyqg_z1CYVVLdPpz1houWkC5JSkKq7WmJnMnLHhRGeJPI2DghPEua1TR6IfkinjPkRyj3lO0aG57LWLBoPYTpdi45VwIrsGxv0n0YHoMFp9wOIdYkoe8rp9KG6Mj_fwhsv_vm58BUwJRJACHyIkf45-cSYuJslZOjqbfXSGeUhKQFcWg83Sn_3q190rKDxwT3SVgdNJS8roQgfZ6FwF_xvzy0lmv68qIt3nIV2I_zz0hnFm00)
- [un altro editor motlo carino per PlantText](https://www.planttext.com/)
- [onlineGDB](https://www.onlinegdb.com/online_c++_compiler)


template:

```
## data

- ...fatto...
- ...fatto...
- ...

```

## 08/08/2022 -- setup della documentazione e project refactoring

la versione precedente del progetto ha parecchi difetti:

- veramente poco intuitiva
- mancanza di un module testing serio
- codice poco chiaro ed eccessivo
- usare ROSPlan come KB è una pessima idea: complica il PDDL e potrebbe dare strani errori

link alla vecchia versione del progetto: [ExperimentalRoboticsLab-Assignment-2-OLD](https://github.com/programmatoroSeduto/ExperimentalRoboticsLab-Assignment-2-OLD.git)

---

prima del primo commit,

- creazione di una nuova repository per il progetto, link [qui](https://github.com/programmatoroSeduto/ExperimentalRoboticsLab-Assignment-2.git)
- craeazione di un template di repository che permetta di usare plantUML (è finito il tempo di scrivere gli UML su draw.io) vedi [qui (privata)](https://github.com/programmatoroSeduto/plantuml-sphinx)
- **primo setup dell'area di lavoro** con plantUML e Sphinx funzionante
- struttura readme, solo elenco puntato per il momento (estendere man mano che andiamo avanti)
- **COMMIT** creazione di un branch vuoto per la copia degli altri eventuali pacchetti da scaricare
- **COMMIT** con tutto il materiale iniziale funzionante

---

*ora siamo pronti per iniziare a lavorare con ROS*. Prima di tutto, il package per il module testing:

- **NUOVO PACKAGE** creazione package per il module testing:
	```
	catkin_create_pkg robocluedo_module_testing roscpp rospy std_msgs geometry_msgs nav_msgs std_srvs
	``` 
- documentazione interna al module testing con index e tutto il resto
- creazione template per la pagina index di un package
- qualche check prima del commit:
	- prova a compilare .. .OK
	- make documentazione .. OK
- **COMMIT** : "module testing package"
- nuova todolist (stavolta su carta)

---

lavoro su aRMOR, estrazione del codice dal vecchio assignment, vedi [erl1](https://github.com/programmatoroSeduto/ExperimentalRoboticsLab-Assignment-1.git):

- download codice sul desktop
- **NUOVO PACKAGE** armor msgs
	```
	catkin_create_pkg robocluedo_armor_msgs std_msgs armor_msgs std_srvs
	```
- senza perdere tempo, prima documentazione
- **NUOVO PACKAGE** armor
	```
	catkin_create_pkg robocluedo_armor roscpp rospy armor_msgs robocluedo_armor_msgs srd_msgs std_srvs
	```
- e prima documentazione anche qui senza perdere tempo
- giusto per, proviamo a compilare, non si sa mai
- **COMMIT** : "armor packages setup"
- importazione del codice delle interfacce aRMOR
	- (header) armor tools
	- (header) armor cluedo
	- (codice) armor tools e armor cluedo
	- (codice nodo) cluedo armor interface
- il primo lavoro è quello di creare i messaggi da compilare, guardando che cosa richiede l'interfaccia nodo armor
	- AddHint srv
	- DiscardHypothesis srv 
	- Hypothesis msg
	- FindConsistentHypotheses srv
- compilaziione messaggi con catkin
- ora devo "solo" aggiornare il codice C++ del nodo ROS e scrivere il catkin
- ora mancano cose tipo ... la ontology ... e tutto il resto
- selezione dei vecchi diagrammi UML riferiti ad aRMOR
- prima di fare altro, proviamo a compilare ... OK! perfetto
- e aggiornamento docs
- **COMMIT** : "armor package first re-adaptation"
- template per le pagine di documentazione con UML
- nuovi diagrammi per aRMOR (quelli che ci sono sono sbagliati...)
	- class diagram iniziale
	- vediamo se genera... 
	- ho dovuto risolvere uno *stupido problema di padding*, vedi `skinparam Padding <tot>`
	- per il momento ci accontentiamo di semplici blocchi
- voglio provare un po' la compilazione esterna di una libreria
	- module testing dal vecchio modulo ... in quello del module testing
	- proviamo a compiare ... non va
		```text
		CMake Error at /opt/ros/noetic/share/catkin/cmake/catkinConfig.cmake:83 (find_package):
		  Could not find a package configuration file provided by "armor_tools" with
		  any of the following names:

			armor_toolsConfig.cmake
			armor_tools-config.cmake

		  Add the installation prefix of "armor_tools" to CMAKE_PREFIX_PATH or set
		  "armor_tools_DIR" to a directory containing one of the above files.  If
		  "armor_tools" provides a separate development package or SDK, be sure it
		  has been installed.
		Call Stack (most recent call first):
		  ExperimentalRoboticsLab-Assignment-2/robocluedo_module_testing/CMakeLists.txt:10 (find_package)


		-- Configuring incomplete, errors occurred!
		See also "/root/ros_ws/build/CMakeFiles/CMakeOutput.log".
		See also "/root/ros_ws/build/CMakeFiles/CMakeError.log".
		make: *** [Makefile:17330: cmake_check_build_system] Error 1
		Invoking "make cmake_check_build_system" failed
		```
	- *per usare le librerie di un package A in un package B basta riferirsi al package A nel package B*
	- proviamo un po' questo test, giusto per essere sicuri
		- questo ... OK (dopo qualche fortissima bestemmia ... prossima volta controlla anche i file inclusi)
			```
			roslaunch robocluedo_module_testing test_armor.launch
			```
		- questo ... OK (ci è voluto poco stavolta, per fortuna)
			```
			roslaunch robocluedo_module_testing test_armor_tools.launch
			```
- voglio un attimo documentare i test in UML, per avere sempre presenti come sono fatti
- **COMMIT** : "testing armor and first documentation"

---

## 09/08/2022 -- modello PDDL e ROSPlan package

- serve uno *schema completo* dell'applicazione prima di iniziare
	- components robocluedo rosplan (solo bozza)
	- *meglio lavorare in MD* : nessuna tabulazione fastidiosa, è più facile lavorarci su
	- components armor 
		- (ricorda di usare l'opzione `allow_mixing` all'inizio del codice per poter usare diversi tipi di UML nello stesso schema)
	- component movement controller
	- **il mission manager non lo faccio adesso** perchè è il nodo che va a "legare" tutte le parti dell'architettura. quindi, meglio lasciarlo a quando avrò un'idea chiara
- **COMMIT** : "uml arch first draft"

---

ora, iniziamo a lavorare su rosplan:

- **NUOVO PACKAGE**
	```bash
	catkin_create_pkg robocluedo_rosplan_msgs std_msgs std_srvs diagnostic_msgs geometry_msgs nav_msgs rosplan_dispatch_msgs rosplan_knowledge_msgs
	```
- documentazione del package e prima configurazione (penso userò quello che ho fatto nella precedente versione, inutile reinventare chissà che)
- e trasferimento dei messaggi nel nuovo package (copia incolla facile facile)
	- e compilazione ... OK
- **NUOVO PACKAGE** 
	```bash
	catkin_create_pkg robocluedo_rosplan roscpp rospy std_msgs std_srvs diagnostic_msgs rosplan_dispatch_msgs rosplan_knowledge_msgs rosplan_dependencies robocluedo_rosplan_msgs
	```
- e primo setup del package
- compilazione da vuoto ... OK
- **COMMIT** : "rosplan first setup"

---

è il momento di rifare il PDDL

- iniziamo col prendere l'environment dal precedente progetto
- mi converrà progettare prima di fiondarmi sul codice...
	- nuovo documento sul PDDL
	- una lista delle azioni PDDL e dei landmarks
	- (ho dovuto aggiornare l'UML di robocluedo rosplan, mi serviva un componente che non avevo ancora menzionato)
	- (l'idea è di fare il design dell'intero PDDL sul documento, in modo da non dover fare due cose assieme, design e implementazione, nella fase di implementazione, *perchè so quanto romperà il planner*)
	- pddl delle varie azioni
- e ora, implementazione
	- prima versione *completamente vuota*
	- parse ... OK (però bisogna vedere se fa il plan o no)
	- plan ... NO ... OK
		domain:
		```lisp
		(define (domain robocluedo)

		(:requirements 
			;; === PDDL1.1 === ;;
			:strips 
			:typing 
			:equality 
			:existential-preconditions
			:universal-preconditions 
			:conditional-effects
			:quantified-preconditions
			
			;; === PDDL2.1 === ;;
			:numeric-fluents
			:durative-actions
		)

		(:types
			;; navigation system
			waypoint
		)

		(:predicates
			(stub ) (not-stub )
		)

		(:functions
			
		)

		(:durative-action stub-true
			:parameters ( )
			
			:duration (= ?duration 0.99 )
			
			:condition (and 
				(at start (not-stub ))
			)
			
			:effect (and
				(at start (not (not-stub )))
				(at end (stub))
			)
		)

		)
		```
		problem:
		```lisp
		(define (problem robocluedo-task)

		(:domain robocluedo)

		(:objects
			
		)

		(:init
			(not-stub )
		)

		(:goal (stub ))

		)
		```
		plan (fino ad ora):
		```text
		root@3b17871017fd:~/ros_ws/src/erl2-new/robocluedo_rosplan/pddl/robocluedo# ./solve_popf.sh 
		Number of literals: 2
		Constructing lookup tables:
		Post filtering unreachable actions: 
		No analytic limits found, not considering limit effects of goal-only operators
		All the ground actions in this problem are compression-safe
		Initial heuristic = 1.000
		;;;; Solution Found
		; States evaluated: 2
		; Cost: 0.990
		; Time 0.00
		0.000: (stub-true)  [0.990]
		```
	- (ora, una sola azione alla volta)
	- replan
		- importazione
		- parse ... OK
		- plan ... OK
			- *precondition e effect richiedono comunque l'operatore and, anche se hanno un solo predicato al loro interno*
		- uml del replan
	- move-to
		- importazione
			- meglio separare `move-to` dall'ottenere l'hint ... nuova azione `signal-collect-hint`
		- parse ...
		- plan ...
		- uml dell'azione implementata in ROS
	- signal-collect-hint
		- importazione
		- parse ...
		- plan ... OK
		
		plan finora: 
		```text
		; States evaluated: 14
		; Cost: 9.909
		; Time 0.00
		0.000: (replan)  [0.990]
		0.991: (move-to center wp1)  [0.990]
		1.982: (signal-collect-hint wp1)  [0.990]
		2.973: (collect-hint wp1)  [0.990]
		3.964: (move-to wp1 wp2)  [0.990]
		4.955: (move-to wp2 wp3)  [0.990]
		5.946: (signal-collect-hint wp3)  [0.990]
		6.937: (collect-hint wp3)  [0.990]
		7.928: (move-to wp3 wp4)  [0.990]
		8.919: (signal-collect-hint wp4)  [0.990]
		```
		per il goal
		```lisp
		(:goal (and (not-dirty ) (at wp4 ) (hint-ready ) (hint-collected wp1) (hint-collected wp3 )))
		```
		
		- uml
	- move to center
		- importazione
		- solve
		
		ultimo output:
		
		```text
		# ./solve_popf.sh 
		Number of literals: 28
		Constructing lookup tables: [10%] [20%] [30%] [40%] [50%] [60%] [70%] [80%] [90%] [100%] [110%] [120%] [130%] [140%] [150%] [160%]
		Post filtering unreachable actions:  [10%] [20%] [30%] [40%] [50%] [60%] [70%] [80%] [90%] [100%] [110%] [120%] [130%] [140%] [150%] [160%]
		No analytic limits found, not considering limit effects of goal-only operators
		93% of the ground temporal actions in this problem are compression-safe
		Initial heuristic = 8.000
		b (7.000 | 0.990)b (6.000 | 0.990)b (5.000 | 2.972)b (4.000 | 5.945)b (3.000 | 6.936)b (2.000 | 7.927)b (1.000 | 8.918);;;; Solution Found
		; States evaluated: 15
		; Cost: 9.909
		; Time 0.00
		0.000: (replan)  [0.990]
		0.991: (move-to center wp1)  [0.990]
		1.982: (signal-collect-hint wp1)  [0.990]
		2.973: (collect-hint wp1)  [0.990]
		3.964: (move-to wp1 wp2)  [0.990]
		4.955: (move-to wp2 wp3)  [0.990]
		5.946: (signal-collect-hint wp3)  [0.990]
		6.937: (collect-hint wp3)  [0.990]
		7.928: (move-to wp3 wp4)  [0.990]
		8.919: (move-to-center wp4 center)  [0.990]
		```
		
		per il goal:
		
		```lisp
		(:goal (and (not-dirty ) (hint-collected wp1) (hint-collected wp3 ) (at-center )))
		```
- **COMMIT** : "working on PDDL (not yet finished)"
- ancora lavoro sul PDDL
	- azione solve
- ora chiariamo la questione dei landmark
- **COMMIT** : "working on PDDL (end of the job)"

---

il pddl di base è pronto. adesso, passiamo al package:

- una descrizione formale della KB
	- un UML sistemato della KB di ROSPlan
	- senza dovermi reinventare nulla, dovrei avere del codice già pronto dalla vecchia repo... 
- **ancora meglio** : ho direttamente le classi!
	- importo il codice nella repo nuova allora, e lo rivedo un attimino
	- il codice va un attimino semplificato... ma soprattutto reso più comprensibile
- kb tools pezzo per pezzo
	- le macro per il log non mi piacciono molto...
		
		```c++
		#define NODE_NAME "kb_tools"

		#ifndef __DEBUG_MACROS__
		#define __DEBUG_MACROS__

		#define LOGSQUARE( str )  "[" << str << "] "
		#define OUTLABEL          LOGSQUARE( NODE_NAME )
		#define TLOG( msg )       ROS_INFO_STREAM( OUTLABEL << msg )
		#define TWARN( msg )      ROS_WARN_STREAM( OUTLABEL << "WARNING: " << msg )
		#define TERR( msg )       ROS_WARN_STREAM( OUTLABEL << "ERROR: " << msg )

		#endif
		```
		
		non mi piacciono perchè fanno spuntare troppi warning inutili...
		
	- via la db mode!
		- un compromesso: piuttosto che avere la variabile al costruttore, posso definire una macro...
	- meglio fare anche il codice in parallelo oltre che l'header
	- per il momento, meglio *eliminare la parte dei fluents* dato che comunque per l'attuale progetto non mi servono. Posso pensare di reintrodurli più avanti.
	- anche la sezione *other queries* si può eliminare, non mi è mai servita nemmeno per il precedente progetto
		- già che ci sono, documento la lettura di un predicate da codice
	- escluderei l'altra libreria... non mi sembra molto utile, e comunque voglio mantenere tutto sul semplice
- prima di andare avanti, *prova a compilare la libreria*
- assumiamo che kb tools funzioni a dovere (dovrebbe, l'ho usata nel vecchio progetto e andava bene)
	- UML di kb_tools
- **COMMIT** : "re-adaptation of kb_tools"
- manca ancora una cosa prima di poter implementare ad esempio il sistema di landmark: *come settare un goal* ?
	- documentazione sui goal
	- implementazione dei due metodi per i goal ... anzi, solo il setter, il getter è noioso da fare e probabilmente non mi servirà
	- aggiornamento degli uml di ROSplan
- compila? compila.
- **COMMIT** : "set goal method"

---

implementazione dell'interfaccia minima con la kb

- launch file per ROSPlan
	- copiato dal vecchio progetto
	- avviare si avvia senza problemi
- già che ci sono voglio fare qualche prova sui goals

problem instance:

```bash
rosservice call /rosplan_problem_interface/problem_generation_server
rostopic echo /rosplan_problem_interface/problem_instance -n 1 -p

```

ottenere l'attuale goal:

```text
# rosservice call /rosplan_knowledge_base/state/goals "predicate_name: ''" 
attributes: 
  - 
    knowledge_type: 1
    initial_time: 
      secs: 0
      nsecs:         0
    is_negative: False
    instance_type: ''
    instance_name: ''
    attribute_name: "dirty"
    values: []
    function_value: 0.0
    optimization: ''
    expr: 
      tokens: []
    ineq: 
      comparison_type: 0
      LHS: 
        tokens: []
      RHS: 
        tokens: []
      grounded: False
  - 
    knowledge_type: 1
    initial_time: 
      secs: 0
      nsecs:         0
    is_negative: False
    instance_type: ''
    instance_name: ''
    attribute_name: "hint-collected"
    values: 
      - 
        key: "wp"
        value: "wp1"
    function_value: 0.0
    optimization: ''
    expr: 
      tokens: []
    ineq: 
      comparison_type: 0
      LHS: 
        tokens: []
      RHS: 
        tokens: []
      grounded: False
  - 
    knowledge_type: 1
    initial_time: 
      secs: 0
      nsecs:         0
    is_negative: False
    instance_type: ''
    instance_name: ''
    attribute_name: "hint-collected"
    values: 
      - 
        key: "wp"
        value: "wp3"
    function_value: 0.0
    optimization: ''
    expr: 
      tokens: []
    ineq: 
      comparison_type: 0
      LHS: 
        tokens: []
      RHS: 
        tokens: []
      grounded: False
  - 
    knowledge_type: 1
    initial_time: 
      secs: 0
      nsecs:         0
    is_negative: False
    instance_type: ''
    instance_name: ''
    attribute_name: "at-center"
    values: []
    function_value: 0.0
    optimization: ''
    expr: 
      tokens: []
    ineq: 
      comparison_type: 0
      LHS: 
        tokens: []
      RHS: 
        tokens: []
      grounded: False

```

- (altre prove più avanti, mi rifiuto di scrivere chissà che sulla shell)
- **NUOVO NODO** : (c++) `kb_interface`
	- uml del nuovo nodo
	- template del codice
	- apertura dell'unico servizio del nodo
- compila? compila.
- ora occupiamoci di fare il replan. l'idea sarebbe di fare un ciclo "stupido" su un tot di predicati con parametri.
	- anzitutto, struttura degli elementi del PDDL nel codice c++
	- scrittura del codice nel costruttore, **PER ORA SOLO INIT**
	- compilare compila (a runtime bisogna vedere...)
	- replan: il ciclo stupido di cui si parlava sopra
- alcune domande:
	- *è necessario resettare anche il goal?* penso proprio di no
	- *serve un servizio per scrivere il goal?* assolutamente sì
- servizio per indicare il goal
	- aggiornamento UML
	- creazione del nuovo tipo di servizio
	- ora, servirà definire i vari predicati (il principio è sempre quello del ciclo stupido)
	- apertura del nuovo servizio
	- compilare compila (ora bisogna vedere se funge)
- un minimo testing per il nodo
	```bash
	roslaunch robocluedo_rosplan load_rosplan.launch
	
	rosrun robocluedo_rosplan kb_interface
	
	rosservice call /rosplan_problem_interface/problem_generation_server
	
	# rostopic echo /rosplan_problem_interface/problem_instance -n 1 -p
	
	rosservice list
	# /update_goal
	# /replan
	```
	azione di replan:
	```bash
	rosservice call /replan "{}" 
	
	rosservice call /rosplan_problem_interface/problem_generation_server
	
	rosservice call /rosplan_knowledge_base/clear 
	
	rosservice call /replan "{}" 
	
	rosservice call /rosplan_problem_interface/problem_generation_server
	
	# rostopic echo /rosplan_problem_interface/problem_instance -n 1 -p
	```
	landmarks:
	```bash
	rosservice call /update_goal "landmark: 0" 
	
	rosservice call /rosplan_problem_interface/problem_generation_server
	
	rosservice call /update_goal "landmark: 1" 
	
	rosservice call /rosplan_problem_interface/problem_generation_server
	
	rosservice call /update_goal "landmark: 2" 
	
	rosservice call /rosplan_problem_interface/problem_generation_server
	
	```
- direi che funziona! bene bene bene
- **COMMIT** : "kb interface first version (seems stable)"

---

sperando che il nodo  appena implementato funzioni a dovere senza sorprese, andiamo avanti col pipeline manager. *stavolta lo voglio implementare in Python*.

- **NUOVO NODO** : rosplan_pipeline_manager.py
	- primo template del nodo (penso stavolta farò un nodo semplice, procedurale, tanto sono solo servizi)
	- subito, UML del nodo e degli elementi principali di ROSplan
	- ora, *usando l'UML che ho appena fatto*, implemento i servizi per il pipeline manager (manca il feedback per ora)
	- ho preparato un minimo template per l'apertura dei client in ROSpy:
		```py
		def open_cl( cl_name, cl_type, SRV_TIMEOUT=60 ):
			'''open a client
			'''
			
			global NODE_NAME
			
			rospy.loginfo(f"({NODE_NAME}) client: {cl_name} ... ")
			try:
				rospy.wait_for_service( cl_name, timeout=SRV_TIMEOUT )
				
			except ROSException as e:
				rospy.logwarn(f"({NODE_NAME}) client: {cl_name} UNABLE TO CONTACT within the timeout ({SRV_TIMEOUT}s) cause: {e}")
				raise e
				
			cl_this = rospy.ServiceProxy( cl_name, cl_type )
			rospy.loginfo("OK")
			
			rospy.sleep(rospy.Duration(0.75))
			
			return cl_this
		```
	- ora che le interfacce le abbiamo, abbozzo un UML del nodo (mancano ancora parecchie cose)
- vediamo un po' di riadattare se possibile il vecchio sistema di feedback ... possibile?
	- posso ... riadattare l'idea, ma il vecchio sistema in se va riadattato
	- partiamo dallo scrivere il messaggio di feedback
	- ora il servizio implementato dal pipeline manager
	- compila? compila, dopo qualche bestemmia
		- ricorda di inserire nel cmake anche *message_runtime* oltre a *message_generation*, altrimenti non si riesce ad importare i messaggi nei servizi nell'ambito dello stesso package
	- (voglio mantenere l'idea del feedback da C++)
	- aggiornamento dell'UML del nodo includendo anche il feedback manager
	- e apriamo il servizio e il topic
- adesso che il nodo ha tutte le interfacce esposte, lavoro per aggiungere le funzionalità una ad una
	- *caricamento del problema*
		- serve ancora una subscription, altrimenti non riesco a capire quando il load non ha avuto successo
		- prima di andare avanti, UML (sarà lunga stasera...)
		- ovviamente manca anche la parte in cui scrivo i landmark nel problema... aggiungere
		- e dopo averla implementata, aggiornare l'UML
	- e test intermedio (mai fidarsi di Python)
		```
		# shell 1
		roslaunch robocluedo_rosplan load_rosplan.launch
		
		# shell 2
		rosrun robocluedo_rosplan kb_interface
		
		# shell 3
		rosrun robocluedo_rosplan rosplan_pipeline_manager.py
		
		# shell 4
		rosservice list | grep robocluedo
		
		rosservice call /robocluedo/pipeline_manager "{load_problem: false, solve_problem: false, parse_plan: false, execute_plan: false, landmark: 0}" 
		
		rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: false, parse_plan: false, execute_plan: false, landmark: 0}" 
		
		rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: false, parse_plan: false, execute_plan: false, landmark: 1}" 
		
		rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: false, parse_plan: false, execute_plan: false, landmark: 2}" 
		
		rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: false, parse_plan: false, execute_plan: false, landmark: 3}" 
		
		rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: false, parse_plan: false, execute_plan: false, landmark: -1}" 
		```
- *per oggi ho sofferto abbastanza*
- **COMMIT** : "working on pipeline manager (adding functionalities)"

---

## 10/08/2021 -- rosplan pipeline manager -- planning e replanning

torniamo a lavorare sul pipeline manager, una funzionalità per volta. Ieri: l'istanza del problema. 

- *soluzione del problema* usando il planner
	- implementazione
	- serve anche aprire il subscriber con il planner per capire quando il plan è andato bene
	- *come capire se il problema non è risolvibile o se c'è stato qualcos'altro che non va?* 
	- per provare, rendo volutamente irrisolvibile il problema:
		```
		(:goal (and (dirty ) (not-dirty ) (hint-collected wp1) (hint-collected wp3 ) (at-center )))
		```
	- output lato rosplan:
		```
		[ INFO] [1660112152.481829500]: KCL: (/rosplan_problem_interface) (OUT.pddl) Generating problem file.
		[ INFO] [1660112152.829298800]: KCL: (/rosplan_problem_interface) (OUT.pddl) The problem was generated.
		[ INFO] [1660112152.829847100]: KCL: (/rosplan_planner_interface) Problem received.
		[ INFO] [1660112162.234649900]: KCL: (/rosplan_planner_interface) (OUT.pddl) Writing problem to file.
		[ INFO] [1660112162.234786600]: KCL: (/rosplan_planner_interface) (OUT.pddl) Running: timeout 10 /root/ros_ws/src/ROSPlan/rosplan_planning_system/common/bin/popf -T /root/ros_ws/src/erl2-new/robocluedo_rosplan/pddl/robocluedo/robocluedo_domain.pddl /root/ros_ws/src/erl2-new/robocluedo_rosplan/data/OUT.pddl > /root/ros_ws/src/erl2-new/robocluedo_rosplan/data/plan.pddl
		[ INFO] [1660112172.240622500]: KCL: (/rosplan_planner_interface) (OUT.pddl) Planning complete
		[ INFO] [1660112172.240715400]: KCL: (/rosplan_planner_interface) (OUT.pddl) Plan was unsolvable.
		```
	- output lato shell:
		```
		root@3b17871017fd:~/ros_ws/src/erl2-new/robocluedo_rosplan/launch# rosservice call /rosplan_problem_interface/problem_generation_server

		root@3b17871017fd:~/ros_ws/src/erl2-new/robocluedo_rosplan/launch# rosservice call /rosplan_planner_interface/planning_server
		ERROR: service [/rosplan_planner_interface/planning_server] responded with an error: b''
		root@3b17871017fd:~/ros_ws/src/erl2-new/robocluedo_rosplan/launch# 
		```
	- il topic non riceve nulla, quindi significa che il plan non è andato a buon fine
	- il goal che ho scelto causa attesa (in questo caso viene sollevata un'eccezione riguadante il server)
	- mi serve un goal che dia contraddizione immediata. questo funziona:
		```
		(:goal (and (not-is-center center )))
		```
	- anche in questo caso non viene inviato nulla 
	- il file generato dal planner contiene questo:
		```
		Number of literals: 28
		Constructing lookup tables: [10%] [20%] [30%] [40%] [50%] [60%] [70%] [80%] [90%] [100%] [110%] [120%] [130%] [140%] [150%] [160%] [170%]
		A problem has been encountered, and the problem has been deemed unsolvable
		--------------------------------------------------------------------------
		The goal fact:
		(not-is-center center)

		...cannot be found either in the initial state, as an add effect of an
		action, or as a timed initial literal.  As such, the problem has been deemed
		unsolvable.
		```
	- per capire se il problema non è risolvibile, occore
		- eliminare il precedente file di plan
		- caricare nel parameter server il percorso dove verrà caricato il file di plan dal planner
		- leggere il file di plan e cercare all'interno di esso la stringa "the problem has been deemed unsolvable"
		- se la stringa c'è, allora il problema è chiaramente irrisolvibile, altrimenti (il file non esiste proprio oppure è diverso) il problema è di altra natura
	- implementazione ...
		- caricamento percorso nel parameter server tramite launch file
		- lavoro col file di plan
		- ora manca solo l'eccezione
	- e urge una prova a questo punto: per ora nel caso del plan unsolvable immediatamente
		```
		# shell 1
		roslaunch robocluedo_rosplan load_rosplan.launch

		# shell 2
		rosrun robocluedo_rosplan kb_interface

		# shell 3
		rosrun robocluedo_rosplan rosplan_pipeline_manager.py
		
		rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: false, execute_plan: false, landmark: 0}" 
		```
	- pare che funzioni. tutti i landmark paiono portare ad una soluzione
	- **ISSUE** (ma basta solo stare attenti): il goal col fallimento immediato `(:goal (and (not-is-center center )))` rimane nel goal, non viene eliminata dalla kb interface. 
- **COMMIT** : "working on pipeline manager (planning interface)"
- *parsing del plan*
	- implementazione (solo trigger per ora)
	- e test ... funge!
- **COMMIT** : "working on pipeline manager (parsing interface)"
- prima di andare avanti mi servono le simulated actions
	- posso implementare una prima versione del dispatcher (per ora senza il feedback)
	- test
		```
		rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 0}" 
		```
- per quanto riguarda il pipeline manager questo è tutto ciò che posso fare ora.
- **COMMIT** : "working on pipeline manager (simulated action and dispatch)"

--- 

passiamo a lavorare sul feedback manager:

- anzitutto lettura del vecchio codice (l'idea è la stessa: una classe che posso usare da padre per altre classi, o che posso istanziare)
- la classe precedente non è più utilizzabile, però posso riadattarla, stesso lavoro di kb_tools direi
	- creazione header e cpp file
- adesso, una cosa per volta, a partire dall'header
	- librerie e "contorno"
	- funzione per la scrittura rapida del messaggio
- per ora basta che compili (i feedback li scrivo man mano che scrivo le azioni)
	- compila? compila.
- UML del feedback manager (per ora senza indicare i tipi di feedback)
- **COMMIT** : "working on feedback manager (first basic implementation)"

---

meglio iniziare a strutturare le implementazioni delle azioni PDDL:

- prima, meglio farsi un template minimo per gli header
	
	header: (action_name.h)
	
	```c++
	
	```
	
	implementazione della classe: (action_name.cpp)
	
	```c++

	```
	
	nodo che implementa la classe: (action\_name_node.cpp)
	
	```c++
	
	```
	
	catkin:
	
	```cmake
	add_library( action_name src/robocluedo_rosplan_actions/action_name.cpp )
	target_link_libraries( action_name ${catkin_LIBRARIES} feedback_manager kb_tools rosplan_action_interface )
	add_executable( node_action_name src/robocluedo_rosplan_actions/action_name_node.cpp )
	add_dependencies( node_action_name ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS} )
	target_link_libraries( node_action_name ${catkin_LIBRARIES} feedback_manager kb_tools rosplan_action_interface action_name )
	```
	
- compila? 
	- [ricorda questo errore!](https://cplusplus.com/forum/general/203321/) piuttosto fastidioso da risolvere
	- dopo aver tribolato mezz'ora, ci siamo
- adesso possiamo partire con l'implementazione delle azioni
- *replan*
	- creazione delle azioni
	- UML dell'azione, giusto per capire che canali aprire
	- implementazione
	- cmake
	- compila? compila.
- *move-to*
	- creazione dell'azione
	- UML della action 
	- manca un servizio per move_to ... 
	- apertura delle interfacce (ne serve solo una)
	- implementazione
	- e cmake
	- compila? compila.
- **COMMIT** : "working on robocluedo_rosplan_actions (replan, move_to)"
- *collect-hint*
	- *pessimo design* : meglio ridimensionare questa azione
	- *manca il servizio manipulation!*
	- creazione dei file per l'implementazione dell'azione
	- implementazione
	- cmake
	- compila? compila.
- *move-to-center* è praticamente identica alla *move-to* di prima
	- uml copiato e incollato
	- l'implementazione è praticamente identica a *move-to*
	- cmake
	- compila? ovviamente compila.
- la *solve* è una simulated action: dato che solo il mission manager può accedere alla ontology, la solve perde di ogni utilità
- **COMMIT** : "working on robocluedo_rosplan_actions (end of the job)"
- implementare (solo per il testing) un nodo che imiti navigation e manipulation
	- **NUOVO NODO** : rosplan_simulated_motion_system
	- implementazione e installazione
	- e un minimo testing
- (manca solo da chiarire il sistema di feedback)
- feedback "hw manipulation failure"
	- ...e tutti gli altri tipi di feedback
- compila? compila
- **gestire meglio l'invio dei feedback**. CI sono vari problemi:
	- il feedback dev'essere inviato *solo quando si presenta un problema* e il pipeline manager dev'essere pronto a riceverlo
	- va inviato solo ed esclusivamente un feedback alla volta
	- il feedback di successo non esiste, è inutile
- ora, tutti i feedback in tutte le azioni
	- in collect hint
	- in move to
	- in move to center
	- in replan
- **COMMIT** : "working on robocluedo_rosplan_actions (feedbacks from the actions)"
- come *ultima fatica per oggi* implementazione dell'esecuzione nella pipeline (test domani)
	- (manca solo la parte riguardande i feedback)
	- giusto un micro test per capire se è tutto a posto
		```bash
		# shell 1
		roslaunch robocluedo_rosplan load_rosplan.launch

		# shell 2
		rosrun robocluedo_rosplan kb_interface

		# shell 3 (not with simulated actions)
		# rosrun robocluedo_rosplan rosplan_simulated_motion_system.py
		
		# shell 4
		rosrun robocluedo_rosplan rosplan_pipeline_manager.py

		rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 2}" 
		```
- **ISSUES** che non potevo notare prima di avere il sistema completo (usando solo simulated actions)
	- (GRAVE) la sequenza dei landmark porta ad un problema irrisolvibile, vedi la combinazione tra i predicati at e explored (probabilmente è un problema della attuale kb interface che non tiene conto di questo conflitto). lo scenario di utilizzo dovrebbe essere: (landmarks) 0 1 1 1 ... 1 2 0 1 ...
	- (minore) la pipeline non segnala il raggiungimento del goal nel messaggio...
	- (minore) il log nella pipeline non segna la fine del dispatch
- **COMMIT** : "working on the pipeline manager (feedback management)"

---

## 11/08/2022 -- implementazione rosplan action -- navigation system -- improving the robot model

anzitutto, vorrei lavorare sulle issue di ieri: il fatto che la sequenza di landmark che mi aspetto non funzioni come si deve è un problema grave che va risolto al più presto. 

- un test più approfondito:

```bash
# shell 1
roslaunch robocluedo_rosplan load_rosplan.launch

# shell 2
rosrun robocluedo_rosplan kb_interface

# shell 3
rosrun robocluedo_rosplan rosplan_pipeline_manager.py

# shell 4
#     landmarks: 0 1 1 1 2
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 0}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 2}"
```

- **ALTRO ASPETTO DA AFFINARE**: il feedback di successo managi non servirà, però va comunque scritto nel messaggio di ritorno.
	- (lo faccio adesso)
	- test ... e ok!
- ora, sostituisco le simulated actions con le mie implementazioni

```xml
<node name="rosplan_interface_???" pkg="robocluedo_rosplan" type="node_???" respawn="false" output="screen">

	<param name="knowledge_base"		value="rosplan_knowledge_base" />
	<param name="pddl_action_name"		value="???" />
	<param name="action_dispatch_topic"	value="/rosplan_plan_dispatcher/action_dispatch" />
	<param name="action_feedback_topic"	value="/rosplan_plan_dispatcher/action_feedback" />
	
	<param name="action_duration"		value="0.99" />
	<param name="action_probability"	value="1.0" />

</node>
```

- e test delle actions reali
	- *mi sono dimenticato di compilare move_to_center!*

```bash
# shell 1
roslaunch robocluedo_rosplan load_rosplan.launch

# shell 2
rosrun robocluedo_rosplan kb_interface

# shell 3
rosrun robocluedo_rosplan rosplan_simulated_motion_system.py

# shell 4
rosrun robocluedo_rosplan rosplan_pipeline_manager.py

# shell 5
#     landmarks: 0 1 1 1 2 0 1 2

rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 0}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 2}"

# ERRORE problema non risolvibile (correttamente segnalato dal pipeline manager)
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 

# problema risolvibile
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 0}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 2}"
```

- (alla fine la issue di ieri era ... semplice sonno)
- **COMMIT** : "working on robocluedo rosplan actions (launch file)"

---

con ROSPlan per il momento dovrei aver finito. passiamo ora a lavorare al movement controller. IDEE:

-  dato che sulla macchina virtuale ci sono così tanti modi di fare navigation col robot, vorrei creare un'interfaccia per usarli tutti, a scelta
- il progetto attuale per il momento *non includerà move_base* per il semplice fatto che in questo contesto non è richiesta la navigazione indoor
- per quanto riguarda moveIt, credo riciclerò tutto dal vecchio progetto ... oppure sarebbe carino usare tf per gestire meglio la posizione dell'end effector, in modo che vada proprio vicino al marker (questo mi darebbe l'opportuunità di approfondire tf2 e simili)

lavoro sul movement controller, anzitutto il modello del robot:

- **IMPORTAZIONE PACKAGE** erl2 (lo starter kit) dal vecchio progetto
	- (importazione in una cartella separata chiamata `robocluedo_dependencies`, in vista di importare anche openCV e simili)
- compila? compila, *e vede correttamente il pakcage sotto la cartella dependencies*
- **COMMIT** : "robocluedo dependencies"
- documentazione dello starter kit (dal precedente assignment)
	- *(mi sono appena reso conto che tale documentazione ... non esiste)*
	- documentazione del nodo "simulation" con UML incluso
		- (c'è voluto più tempo del previsto ...)
- importazione del modello del robot dal precedente package (solo URDF per ora...)
	- nuova cartella `robocluedo_dependencies/robocluedo_urdf_model` con tutti gli strumenti di sviluppo del caso
	- con solito environment standard (al terzo progetto ce la facciamo...)
	- e documentazione (un male necessario)
	- *il precedente modello è troppo disordinato ...* meglio mettere in ordine, iniziando col separare i componenti di Gazebo (e altri lavori)
	- ho solo scomposto il modello, dovrebbe continuare a funzionare... e funziona infatti (meno male)
	- e torniamo sulla documentazione (per ora molto minimal)
- **COMMIT** : "working on robot model (first setup, xacro model)"
- e adesso il package in MoveIt
	- prima creazione col setup assistant
	- il gripper non funziona bene ... tento di correggerlo ... 
	- (anche qui c'è voluto più del previsto)
- e facciamo funzionare il package MoveIt
	- seguendo la procedura sulle slides...
	- funziona? **funziona!**
- **COMMIT** : "working on robot model (moveit package, fixes on the urdf model)"
- mancano ancora alcune cose:
	- world files
	- piccola prova ... ok!
	- meglio salvarsi delle immagini dell'environment
- **COMMIT** : "working on robot model (world files)"

---

ora, il lavoro sul movement controller

- **CREAZIONE NUOVO PACKAGE** : 
	```bash
	catkin_create_pkg robocluedo_movement_controller_msgs std_msgs std_srvs geometry_msgs moveit_msgs nav_msgs
	```
- e prima impostazione della documentazione
- compila? e meno male
- **CREAZIONE NUOVO PACKAGE**:
	```bash
	catkin_create_pkg robocluedo_movement_controller roscpp rospy geometry_msgs std_msgs std_srvs nav_msgs actionlib actionlib_msgs moveit_msgs moveit_core moveit_ros_perception moveit_ros_planning moveit_ros_planning_interface robocluedo_movement_controller_msgs
	```
- al solito, prima documentazione
- mi da fastidio indicare i world files usando i path assoluti, per questo ho creato un finto package worlds usando i cmake files
	- modifica ai file moveit
	- prova ... funziona
- (ora che ho tutto, inizio col navigation system)
- schema UML del navigation system, giusto per avere le idee chiare
	- wall follow
	- go to point
	- bug_m (e tutto l'ensemble)
- **LIMITAZIONI** dei nodi attuali per la navigation
	
	- il wall follow va bene, non serve fare nulla
	- il go to point dovrebbe stabilire un'orientazione finale
	- (go to point) creare un servizio *normale* per la gestione del movimento, che faccia rimanere in attesa fin quando il robot non ha raggiunto il punto indicato
	- (go to poit) oppure creare un topic che permetta di verificare lo stato del robot
	- (go to point) per quanto riguarda l'orientazione potrei aggiungere semmai un servizio molto simile a quello che già c'è per l'orientazione
	
	la strategia migliore sarebbe la seguente:
	
	- serve un nodo che implementi un'interfaccia più intuitiva col navigation system: quella attale va bene, può funzionare, ma non è troppo intuitiva
	- implementare l'orientazione come un behaviour a parte
	- per l'angolo si può sempre implementare un sistema tipo parameter server

- **NUOVO NODO** : (py) head_orientation.py
- (vorrei mantenere l'approccio iniziale dell'implementazione del behaviour, trovo sia un approccio interessante)
- implementazione del nodo head orientation
- studio bug_m (devo trovare un modo furbo e rapido di modificarlo)
- nuovo stato in bug_m
- uml vari ed eventuali
	- uml del nuovo nodo
	- e altri UML
- **NUOVO COMMIT** : "working on the navigation system (first version, not yet tested)"
- adesso serve un po' di testing:

```bash
# shell 1
roslaunch robocluedo_robot gazebo.launch

# shell 2
rosrun robocluedo_movement_controller wall_follow_service_m.py

rosrun robocluedo_movement_controller go_to_point_service_m.py

rosrun robocluedo_movement_controller head_orientation.py

rosrun robocluedo_movement_controller bug_m.py
```

- test completo (conviene creare un launch file)
- mancava l'active nel bug_m
	- aggiornamento UML di bug_m 
- **ISSUE** : tutti i nodi devono essere *spenti* all'avvio del sistema
	- **ISSUE** : il servizio, quando dovrebbe disattivare il nodo, *non invia il segnale di stop!*

- comunque, a parte qualche difetto di meccanismo, *sembra funzionare*
	- l'unica cosa davvero da risolvere è rifare la ruota davanti del robot
- **COMMIT** : "working on the navigation system (testing)"

---

- **ISSUE** : il robot si incastra per terra ... perchè quella cosa sul frontale del robot *non è una vera castor wheel*
	- anzitutto, la situazione, con un teleop twist keyboard stupido stupido giusto per vedere come si muove il robot
	- ho provato ad aumentare la dimensione della "castor" davanti, giusto per avere una superficie di impatto meno aspra e sembra funzionare
	- torno alla vecchia giusto per confrontare... con la ruota piccola il robot tende ad impuntarsi in avanti
	- potrebbe anche essere un problema di guadagno: per il movimento bisogna aumentare la velicità
- (voglio mettere esplicitamente a confronto le due situazioni)
- *robot con la ruota piccola*
	- test giro su se stesso
		- il robot si impunta e tende a ruotare attorno alla castor
	- test andare avanti e tornare indietro
		- andando avanti il robot tende ad oscillare perchè si impunta sulla ruota, viene fuori un movimento di pitch che non dovrebbe esserci e che disturba la odometry
	- test girare in cerchio
		- tende a non girare
- **il braccio deve essere sollevato durante i test!** altrimenti certo che il secondo caso è meglio del primo: il braccio è piegato all'indietro!
- *e ora robot con la ruota grande*, aumentando solo la dimensione della ruota senza alterarne l'origine
	- test giro su se stesso
		- molto meglio, anche se non perfetto
	- test andare avanti e tornare indietro
		- pitch davvero eccessivo...
	- test girare in cerchio
		- il comportamento è ancora ondeggiante, però stavolta il robot tende a fare un giro più stretto, non si perde il giro
- guardando il robot però mi sta venendo un'ideuzza: *ma se lo facessi avanzare dalla parte sul retro piuttosto che dalla dua parte frontale?* lo faccio andare in retromarcia piuttosto che in avanti come dovrebbe, così la castor la trascina e basta, senza corrompere così tanto il robot, e potrei anche ripristinare la vecchia ruota
	- ripristino della vecchia ruota e "tenta di invertire"
	- *ha funzionato!* sono molto soddisfatto del risultato 
	- (basta solo non andare troppo veloce o il robot si ribalterà in avanti)
	- *maaa se gli mettessi altre due ruote?* con dei joint non controllati
	- *anche questa funziona!* e allora sai che c'è? gli tolgo la castor adesso
	- già che siamo in vena di aggiornamenti, voglio provare a mettere a posto il gripper (o eliminarlo direttamente?)
- come temevo ho fatto casino: devo rifare il pacchetto moveit...
	- (dopo un'ora di bestemmie piuttosto forti...)
- **COMMIT** : "working on robot model (improving the stability of the robot)"

---

## 12/08/2022 -- ancora sul movement controller

- anzitutto, i lavori noiosi:
	- aggiornamento delle immagini nella doumentazione
	- aggiornamento dei file yaml salvati nella documentazione del robot
	- piccola modifica ai launch file del robot per supportare anche i percorsi dei world files
- **COMMIT** : "working on robot model (gazebo fix, documentation)"

---

il robot potrebbe avere anche diversi sistemi di navigation, perciò serve un'interfaccia che tenga conto del particolare funzionamento dell'algoritmo di motion planning attualmente in uso, e che eventualmente permetta di cambiare l'algoritmo. 

- anzitutto, mi serviranno delle nuove interfacce
	- un service per position e orientation del robot
	- un service che permetta di scegliere quale algoritmo usare
- **NUOVO NODO** : (c++) navigation_manager.cpp
	- per l'implementazione di questa classe vorrei un sistema modulare, nel senso che scrivo a parte il codice, poi il navigation controller se lo prende e lo esegue; vorrei che tutte le implementazioni condividessero la stessa interfaccia. Il problema è che questo complicherebbe molto di più il lavoro, perciò ricorrerò ad un polimorfismo "dubbio" per il momento, in vista di ristrutturarlo più avanti
	- (sperando di non essere caduto in qualche stupido pitfall del C++ com'è successo l'ultima volta col simple bridge)
	- prima di fare qualunque cosa, proviamo se almeno compila ... 
	- (*mai mischiare reference con pointers...* per il momento meglio i soli semplici pointers, amiamo il rischio da queste parti)
	- interfaccia ROS per la selezione dell'algoritmo
	- compila? compila.
- (ora, iniziamo ad implementare qualche algoritmo)
- iniziamo da bug_m
	- definizione della classe
	- interfacce ROS del controller per comunicare con bug_m
	- prima di compiare, voglio essere sicuro che tutti i servizi siano *disattivati* all'avvio del controller, quindi piccola modifica a bug_m per forzare la situazione
	- compila? compila.
- **COMMIT** : "working on navigation (nav manager first version and first debug, yet not tested)"
- direi che prima di andare avanti bisogna per forza fare del testing, molto semplice
	- **DIFETTO** ci sono degli strani output da wall follow, pensavo di averli tolti tutti
	- **ISSUE** in bug_m bisogna aspettare che qualcuno fornisca il servizio per la user interface
	- **ISSUE** casino apocalittico sui servizi di bug_m ... ci starò un po'
	- (il problema con python si è risolto magicamente ... si era piantato ... ecco perchè detesto usare py per questo genere di lavori)
		- SCHEEEEEERZONE
	- **ISSUE** il nodo c++ non è in grado di gestire due service se non ha abbastanza thread ... async spinner
- **COMMIT** : "working on navigation (debugging... there's a problem with go_to_point and bug_m)"

```bash
# shell 1
roslaunch robocluedo_robot gazebo.launch 2>/dev/null

# shell 2
roslaunch robocluedo_movement_controller navigation_system.launch

# shell 3
rosrun robocluedo_movement_controller navigation_manager

#shell 4
rosservice call /navigation_manager/set_algorithm "algorithm: 0
enabled: false"

rosservice call /navigation_manager/navigation "target:
  x: 1.5
  y: 2.0
  yaw: 1.0
force_enable: false" 

rosservice call /navigation_manager/set_algorithm "algorithm: 0
enabled: true"

rosservice call /navigation_manager/navigation "target:
  x: 1.5
  y: 2.0
  yaw: 1.0
force_enable: false" 

```

---

c'è un problema piuttosto fastidioso col service go to point che non riesco a capire (al solitio l'unico modo per debuggare il codice python è ... eseguirlo, con tutte le grane del caso). A quanto ne so potrebbe anche essere un typo.

- il problema potrebbe essere in go to point ... voglio provarlo da solo
	- no, go to point funziona bene ...
- allora il problema potrebbe essere in bug_m

l'errore in questione me lo segno qui:

lato bug_m:

```text
process[go_to_point_service_m-1]: started with pid [3888]
process[wall_follow_service_m-2]: started with pid [3889]
process[head_orientation-3]: started with pid [3890]
process[bug_m-4]: started with pid [3891]
Wall follower - [2] - follow the wall
[ERROR] [1660306330.473755, 452.647000]: Error processing request: 'NoneType' object is not callable
Traceback (most recent call last): 
   File "/opt/ros/noetic/lib/python3/dist-packages/rospy/impl/tcpros_service.py", line 633, in _handle_request
    response = convert_return_to_response(self.handler(request), self.response_class)
   File "/root/ros_ws/src/erl2-new/robocluedo_movement_controller/scripts/bug_m.py", line 129, in bug_switch
    change_state(2)
   File "/root/ros_ws/src/erl2-new/robocluedo_movement_controller/scripts/bug_m.py", line 109, in change_state
    srv_client_user_interface_() 
TypeError: 'NoneType' object is not callable
Yaw error: [0.03443149860346817]
Position error: [0.2979991913211027]

```

lato navigation manager:

```text
[ INFO] [1660306330.440827200]: [navigation_manager] starting ... 
[ INFO] [1660306330.443291200]: [navigation_manager] Advertising service [/navigation_manager/set_algorithm] ...
[ INFO] [1660306330.445242500]: [navigation_manager] Advertising service [/navigation_manager/set_algorithm] ... OK
[ INFO] [1660306330.445294500]: [navigation_manager] Advertising service [/navigation_manager/navigation] ...
[ INFO] [1660306330.447810400]: [navigation_manager] Advertising service [/navigation_manager/navigation] ... OK
[ INFO] [1660306330.447872900]: [navigation_manager] Opening client [/bug_switch] ...
[ INFO] [1660306330.448978900]: [navigation_manager] Opening client [/bug_switch] ... OK
[ INFO] [1660306330.449069900]: [navigation_manager] advertising service [/bug_m_signal] ...
[ INFO] [1660306330.450628400]: [navigation_manager] advertising service [/bug_m_signal] ... OK
[ERROR] [1660306330.475950600]: Service call failed: service [/bug_switch] responded with an error: error processing request: 'NoneType' object is not callable
[ INFO] [1660306330.476083500]: [navigation_manager] ready

```

- guardando meglio l'errore potrei aver risolto ... 
	- prova ... nada, sempre lo stesso errore (però adesso almeno ce n'è uno solo che si ripete, e non cambia ogni volta)
	- l'errore `TypeError: 'NoneType' object is not callable` significa che il client non viene aperto, perchè la funzione in effetti vede la variabile correttamente
	- il servizio incriminato è sicuramente `/bug_m_signal`
	- come viene aperto in cpp?
	- il problema si è risolto aggiungendo un'attesa, evidentemente python è un po' più lento, e questo genera problemi di sincronizzazione
- ok, ora il problema è risolto ... però adesso *il robot non si muove*
	- c'è un problema col nodo cpp
	- lo switch di bug_m pare non funzionare...
	- c'è un problema con l'apertura dei servizi in Python...
	- proviamo aggiungendo qualche timeout... 
	- **BECCATO!** In effetti bug_switch non funziona come si deve, la chiamata non ha effetto
	- vediamo se riesco a capire qualcosa con qualche log in più

```bash
# shell 1
roslaunch robocluedo_robot gazebo.launch 2>/dev/null

# shell 2
roslaunch robocluedo_movement_controller navigation_system.launch

# shell 3
rosparam set des_pos_x 2.0
rosparam set des_pos_y -2.0
rosparam set des_yaw 2.3
rosservice call /bug_switch "data: true" 
sleep 15
rosservice call /bug_switch "data: false" 
rosparam set des_pos_x -2.0
rosparam set des_pos_y 2.1
rosparam set des_yaw 0.47
rosservice call /bug_switch "data: true" 
sleep 15
rosservice call /bug_switch "data: false" 

```

- il servizio arriva, l'informazione arriva corretta, ma il nodo non si attiva
- **C'E' UN PROBLEMA COL LASER!** assolutamente insospettabile... ecco perchè ci stavo girando così tanto attorno
	- in effetti il robot pubblica il laser su '/m2wr/laser/scan' e non su '/scan'
	- anche in vista di usare move_base meglio usare direttamente '/scan'
- e proviamo di nuovo ... **E FUNZIONA** almeno solo la parte riguardante python
	- c'era un errore nel nuovo stato 3, una cosa che non avevo notato prima, anche se nulla di serio
- ora la parte python pare funzioni
- adesso, proviamo la parte cpp
	- **E ADESSO FUNZIONA**
- **COMMIT** : "working on navigation (debugging, now it works, laser issue)"

---

assumendo che tutto funzioni per il verso giusto ... passiamo all'implementazione della parte moveit:

- innanzitutto, un movement controller per moveit già ce l'ho, riciclo
	- serve definire l'interfaccia
	- implementazione del nodo
	- cmake e .. compila
- test, giusto per capire se funziona

```bash
# shell 1
roslaunch robocluedo_robot demo_gazebo.launch 2>/dev/null

# shell 2
rosrun robocluedo_movement_controller manipulation_controller 2>/dev/null

```

- funziona, ma c'è qualche problemino col controller, regolo il parametro p dei controller, vediamo se cambia qualcosa
- HA FUNZIONATO! benissimo!
- **COMMIT** : "working on manipulation (manip controller, arm tuning)"

---

e il movement controller è finito, o almeno la prima versione. Adesso possiamo iniziare a pensare al mission manager.

- **NUOVO PACKAGE** :
	```bash
	catkin_create_pkg robocluedo_mission_manager rospy roscpp robocluedo_rosplan_msgs robocluedo_movement_controller_msgs robocluedo_armor_msgs std_msgs geometry_msgs sensor_msgs nav_msgs
	```
- e prima documentazione del package (vuota)
- iniziamo dalla parte più "semplice": movimento e comandi del braccio
- **NUOVO NODO** : manipulation unit
- **COMMIT** : "working on mission manager (manipulation unit first setup)"
- implementazione easy-pisy lemon sequeezy della manipulaton unit
	- cmake e test ... funge,funge
- **COMMIT** : "working on mission manager (end manipulation unit)"
- **NUOVO NODO** : navigation unit (prima o poi sarebbe giunto questo momento)
	- template nodo
	- e implementazione
	- cmake e compila ... compila!
- **COMMIT** : "working on mission manager (navigation unit, first debugging but yet not runned)"

---

*e perchè non concludere la nottata in bellezza?* Iniziamo subito ad occuparci del (temibilissimo) mission manager, *quello vero*, che sarà un nodo scritto in C++ che fungerà da centro di tutta l'architettura. 

- **NUOVO NODO** : (c++) mission_manager
	- primo template e design del nodo
	- *devo fare gli schemi mancanti per aRMOR... che p...* li faccio ora subito e non ci pensiamo più
	- e torniamo a lavorare sull'interfaccia...
	- (è stata una cosa lunga...)
	- compila? (dopo qualche colpo di sranga) compila.
- **COMMIT** : "working on mission manager (design of the main node)"

---

## 13/08/2022 -- mission manager -- module testing -- test completo

implementazione del nodo main dell'architettura (fatto questo si passa al testing e si impacchetta tutto)

- implementazione
- non ricordo come funzionassero i servizi di aRMOR e quale fosse la semantica dei messaggi di aRMOR interface... mi tocca documentarli
- **LIMITAZIONE** in aRMOR questa volta: nella add hint l'ID è un numero intero, e la cosa mi sta bene, ma in tutti gli altri metodi l'ID è in realtà una stringa, e anzi, tutte le volte la ontology utilizza la stringa, e non l'intero, per identificare l'ipotesi. 
	- *è il momento di imparare a parsare stringhe in C++* (can't wait for it)
- giusto un mini programmino di prova: (più che altro per sondare le mie caopacità dinamitarde)

	```c++
	#include <iostream>
	#include <string>
	#include <stdlib.h>
	#include <unistd.h>

	int main()
	{
	  std::string id = "ID13398";
	  
	  std::size_t pos_init = id.find("ID");
	  std::cout << "pos=" << pos_init << std::endl;
	  // std::size_t pos_end = pos_init + 2;
	  std::cout << "(str) ID=" << id.substr(pos_init+2) << std::endl;
	  // int id_int = atoi(id.substr(pos_init+2).c_str());
	  int id_int = atoi(id.substr(2).c_str());
	  std::cout << "(int) ID=" << id_int << std::endl;
	  
	  return 0;
	  
	  /*OUTPUT
	  
	  pos=0
	  (str) ID=13398
	  (int) ID=13398
	  
	  */
	}

	```

- posso fare il parsing direttamente dalla parte del nodo che usa aRMOR piuttosto che fare tutto in aRMOR. *è perfettamente sensato che aRMOR usi stringhe anzichè identificatori interi* a meno di non voler  modificare l'ontologia. basta assicurarsi che gli identificatori abbiano *sempre* una data struttura
- a quanto pare aRMOR interface attualmente forza l'identificatore a essere strutturato come *HP<id>*, ad esempio "HP6" o "HP41", vedi implementazione di AddHint ...
	- ... e direi, *a scanso di equivoci* di lasciare tutto così
	- casomai posso solo aggiungere un checking (attualmente mancante) sulla *positività dell'ID fornito nel messaggio*. 
		- lo aggiungo ora
- e terminiamo la documentazione
- ma aspetta: *se tanto non posso modificare i nomi delle ipotesi, che senso ha fornire la stringa?* a questo punto direi che posso modificare direttamente le interfacce per gli identificatori interi, e fine. senza complicare ulteriormente la mia esistenza con ulteriori non necessarie convenzioni sui nomi...
	- AddHint (tra l'altro Aelem non veniva nemmeno usato qui...)
	- find hint 
	- discard hint
	- prima di riprendere a fare qualunque altra cosa direi che posso provare a compilare, no? compia? ... E COMPILA SIGNORI
- **COMMIT** : "working on armor interface (forcing hidden naming convention)"
	- **HO FATTO CASINO CON GIT** e ora devo risolvere...
	- casino coi large files ... ho dovuto fare backup, `git reset --hard <commit>` e aggiornamento, c'era un commit che continuava a "voler essere" caricato su git
- proviamo a compilare di nuovo, *non si sa mai* ... e anche questa volta ce l'abbiamo fatta
- dopo questa (tutt'altro che veloce) divagazione, torniamo sulla portata principale di oggi
	- implementazione dell'acquisizione dell'hint
- e passiamo agli stati
	- anzitutto, il protocollo per determinare un fault
	- anzi, meglio fare un po' di design per commenti per adesso, identificando le parti più semplici
- e adesso, implementazione precisa stato per stato (sarà una cosa lunga e dolorosa...)
	- implementazione di MISSION_STATUS_REPLAN
		- ma prima, un po' di delirio coi messaggi per la rosplan pipeline...
	- (ROBA BRUTTA)
	- implementazione di MISSION_STATUS_COLLECT
		- al secondo passaggio fortunatamente inizio ad avere più codice su cui lavorare ... e parecchio materiale già pronto, dovrei andare più veloce
	- (ROBA BRUTTISSIMA)
	- implementazione di MISSION_STATUS_ASK_ONTOLOGY
	- ... (e tutte le altre)
- e fine! (almeno, senza provare a compilare)
- **COMMIT** : "working on mission manager (painful, endless implementation of the main mission manager)"

---

e ora la parte ... migliore? *il debugging dell'ecomostro*

- vediamo se riusciamo a farlo compilare ... (domenica libera? magari!)
- (posso dire che mi aspettavo di peggio? non sono tantissimi, sono sollevato)
- **ammazzando selvaggiamente** gli ultimi errori
- (molto soddisfatto, e fortuna che ho pensato di implementarlo in C++ ... l'avessi fatto in python sarei diventato pazzo da manicomio)
- **COMMIT** : "working on mission manager (first debugging of the mission_manager node)"

---

il progetto completo adesso c'è! i prossimi passi? module testing, e infine test finale dell'intero progetto. (quella domenica libera potrebbe non essere solo un sogno ... a meno di non aver combinato un casino apocalittico)

- prima di tutto, a posto la documentazione su come eseguire il progetto e come testarlo (ci sono due documenti fatti apposta per questo)
	- inizio già ad elencarmi i test da fare
- test per aRMOR
	- armor puro -- module testing
	- test armor tools
		- ho dovuto aggiornarlo leggermente: non c'era nulla di errato, solo però c'erano i due test di armortools e armorcluedo nello stesso file; ora sono divisi
		- *e meglio scrivere gli output dei test in fondo al file...* altrimenti presto non ci si capirà nulla
		- l'ultima query mi da un'eccezione java nella ontology, pare che esista un oggetto null, ma non saprei perchè
		- controllando le query sembra tutto a posto, non ce lo metto io di sicuro quell'oggetto
		- (non penso che ci sia qualcosa di strano, però non mi convince ... è anche vero però che il test è molto vecchio)
		- scrissi questa cosa nel test:
		
			```text
			* classe più profonda di un individual (caso bastardo, probabilmente bug)
			* 	"QUERY", "CLASS", "IND", "HP3", "true"
			* 	HYPOTHESIS
			```
			
			quindi questo comando non funzionava già prima. tutto a posto: o ho trovato un workaround per farlo funzionare comunque, o ho proprio dribblato la questione. In ogni caso, devo aver sicuramente risolto in passato il problema. Attualmente, basta commentare la riga incriminata e siamo tutti contenti.
		
		- giusto per la cronaca, l'errore è questo:

			```text
			 13/08/22_12:31:13,855 -> Class OWLReferencesInterface : [[ !!! ERROR !!! ]]Cannot get the OWL name of a null OWL object
			Exception in thread "pool-1-thread-22" java.lang.IllegalArgumentException
				at com.google.common.base.Preconditions.checkArgument(Preconditions.java:108)
				at org.ros.internal.message.field.PrimitiveFieldType$14.serialize(PrimitiveFieldType.java:574)
				at org.ros.internal.message.field.ListField.serialize(ListField.java:62)
				at org.ros.internal.message.DefaultMessageSerializer.serialize(DefaultMessageSerializer.java:32)
				at org.ros.internal.message.DefaultMessageSerializer.serialize(DefaultMessageSerializer.java:26)
				at org.ros.internal.message.field.MessageFieldType.serialize(MessageFieldType.java:92)
				at org.ros.internal.message.field.ValueField.serialize(ValueField.java:62)
				at org.ros.internal.message.DefaultMessageSerializer.serialize(DefaultMessageSerializer.java:32)
				at org.ros.internal.message.DefaultMessageSerializer.serialize(DefaultMessageSerializer.java:26)
				at org.ros.internal.node.service.ServiceRequestHandler.handleRequest(ServiceRequestHandler.java:66)
				at org.ros.internal.node.service.ServiceRequestHandler.access$100(ServiceRequestHandler.java:38)
				at org.ros.internal.node.service.ServiceRequestHandler$1.run(ServiceRequestHandler.java:99)
				at java.util.concurrent.ThreadPoolExecutor.runWorker(ThreadPoolExecutor.java:1149)
				at java.util.concurrent.ThreadPoolExecutor$Worker.run(ThreadPoolExecutor.java:624)
				at java.lang.Thread.run(Thread.java:748)
			```
		
	- test armor cluedo
- test su robocluedo rosplan
	- anzitutto voglio spostare il nodo py che simula le interfacce nel module testing
	- test PDDL (non si sa mai...)
		- *fortunatamente* il modello è ok
	- simple rosplan
	- knowledge base interface (vedi diario di implementazione ... visto a cosa serve tenere dei diari?)
	- pipeline manager (anche qui dai diari)
	- tutto senza roslaunch
		- "piccolo" problema con un manifest che non andava bene ... risolto (c'ho messo più del previsto...)
	- a questo punto direi che possiamo scriverlo 'sto launch finale, no?
	- e test finale
- (boh pare funzionare tutto a dovere .-.. ci fidiamo?)
- test sul package del robot
	- urdf
	- robot
- ora, i test sulla navigation (e qui sono piuttosto curioso)
	- go to point
	- head orientation
	- wall follow
		- **ISSUE** il wall follow non si disattiva ... a vedere il comportamento del robot pare che non riesca a pubblicare la posizione dopo che è stato disattivato, ma il ciclo rimane in funzionamento, cosa strana 
		- tutto risolto: c'era un bug nella disattivazione (dal nodo originale)
	- e ora, quello che tutti stavamo aspettando! bug_m!
	- ora il nav controller
		- **ISSUE** : credo ci sia un problema con il controllo della yaw da parte del controller: certe volte va, altre no, e non mi è chiaro il perchè. 
		- ho notato che il robot manda il segnale *prima di aggiustare la yaw*, e sapendo come ho strutturato gli stati, è presto detto il perchè
		- avevo ragione, c'era un passaggio di stato poco pensato. problema *risolto*
	- manca solo il manipulator, per cui secondo me ho già un test bello pronto
- **COMMIT** : "module testing (rosplan, navigation, manipulation)"

---

e adesso siamo giunti al momento di *provare la comunicazione tra i component*. Prima di provare il mission manager per intero voglio essere assolutamente certo che tutto quello che c'è attorno al mission manager funzioni, in modo da poter isolare il problema più facilmente.

- (proseguiamo col module testing)
- rosplan più controller!
- ... ok, avevo sottovalutato la situazione: *serve un launch file globale* altrimenti devo lanciare una ricca costellazione ... di shell, e non ne ho voglia
- test ... ma niente
	
	questo errore è un autentico mistero ... 
	
	```text
	root@3b17871017fd:~/ros_ws/src/erl2-new# rosrun robocluedo_mission_manager navigation_unit
	[ INFO] [1660407077.831806900]: [navigation_unit] starting ... 
	[ INFO] [1660407077.839691200]: [navigation_unit] sub topic /visualization_marker ... 
	[ INFO] [1660407077.842722600]: [navigation_unit] sub topic /visualization_marker ...  OK
	[ INFO] [1660407077.842797900]: [navigation_unit] advertising service /robocluedo/navigation_command ... 
	[ INFO] [1660407077.843474100]: [navigation_unit] advertising service /robocluedo/navigation_command ... OK
	[ INFO] [1660407077.843544000]: [navigation_unit] opening client /navigation_manager/navigation ... 
	[ INFO] [1660407077.844301900]: [navigation_unit] opening client /navigation_manager/navigation ... OK
	[ INFO] [1660407077.844362200]: [navigation_unit] opening client /navigation_manager/set_algorithm ... 
	[ INFO] [1660407077.845099500]: [navigation_unit] opening client /navigation_manager/set_algorithm ... OK
	[ INFO] [1660407077.845175600]: [navigation_unit] ready
	[ INFO] [1660407111.381570100, 178.948000000]: [navigation_unit] received a request (wp=)wp1
	[ INFO] [1660407111.381646300, 178.949000000]: [navigation_unit] enabling motion planning algorithm...
	[ERROR] [1660407111.381688800, 178.949000000]: Call to service [/navigation_manager/set_algorithm] with md5sum [0e1a5befb7c98ef5415a7d22fbb30177] does not match md5sum when the handle was created ([6e1d2f861340c9704c6bdc4be6b7a162])
	[ WARN] [1660407111.381745700, 178.949000000]: [navigation_unit] WARNING: unable to contact the service to enable the navigation algorithm.

	```
	
	il problema (se ho capito bene) significa che la navigation unit non riesce a contattare il navigation manager (almeno il servizio riguardante l'algoritmo) ... però il servizio pare essere stato correttamente attivato, quindi non ha molto senso. 

- provo a riavviare i componenti e a vedere se il servizio viene definito
	- la stessa chiamata da console ha successo, quindi dev'esserci qualcosa che non va col nome del servizio, o insomma con la definizione del servizio nella navigation unit

		```text
		root@3b17871017fd:~/ros_ws# rosservice list | grep algo
		/navigation_manager/set_algorithm
		root@3b17871017fd:~/ros_ws# rosservice info /navigation_manager/set_algorithm 
		Node: /navigation_manager
		URI: rosrpc://3b17871017fd:55987
		Type: robocluedo_movement_controller_msgs/Algorithm
		Args: algorithm enabled
		root@3b17871017fd:~/ros_ws# rosservice call /navigation_manager/set_algorithm 
		Usage: rosservice call /service [args...]

		rosservice: error: Please specify service arguments
		root@3b17871017fd:~/ros_ws# rosservice call /navigation_manager/set_algorithm "algorithm: 0
		enabled: true" 
		success: True
		details: ''
		root@3b17871017fd:~/ros_ws# 
		```
	- **BECCATO!** Il problema stava proprio nella definizione, in particolare il tipo di messaggio con cui è stato definito il servizio: dovrebbe essere `Algorithm`
		
		```text
		// client navigation algorithm
		TLOG( "opening client " << SERVICE_SET_ALGORITHM << " ... " );
		cl_nav_algorithm = nh.serviceClient<robocluedo_rosplan_msgs::NavigationCommand>( SERVICE_SET_ALGORITHM );
		if( !cl_nav_algorithm.waitForExistence( ros::Duration(60) ) )
		{
			TERR( "unable to contact the server " << SERVICE_SET_ALGORITHM << " - timeout expired (60s) " );
			return;
		}
		TLOG( "opening client " << SERVICE_SET_ALGORITHM << " ... OK" );
		```
		
		e c'era anche un altro errore: da un'altra parte chiamavo `cl_nav_algorithm` anzichè `cl_nav`, ma essendo la definizione del servizio errata, per il compilatore non c'era problema.
	
	- (sonno fortissimo mentre scrivevo 'sta roba, c'erano un sacco di altri errori stupidissimi)

- eeee proviamo di nuovo, stesso test (diamine, era riba già collaudata) ... 
- **ISSUE** i marker sono messi in posizioni irraggiungibili dal robot (questa è la parte corretta) ...
	- ... ma *il robot non dovrebbe andare proprio così vicino* ... come si risolve questa cosa *senza implementare un qualcosa di troppo customizzato?*
	- temo non ci sia altra scelta: devo scalare la posizione all'interno del nodo bridge... non mi piace molto
- e proviamo ancora
- **ISSUE** angolo sbagliato, e coordinate decisamente troppo lontane dal punto (maaa dormivo quando ho scritto quella roba? *probabile*)
	- potrei aver sottovalutato la questione. Per avere un approccio migliore, almeno in questa situazione, e data la pessima controlalbilità del robot, potrei pensare ad un movimento di avvicinamento al marker in 3 fasi: avvicinamento del 75% (mi piazzo di fronte al marker), avvicinamento *rettilineo* al 90%, e dopo il movimento col manipolatore, allontanamento in retro al 75%
	- il problema è che il robot attuale *non supporta la retro...*
- almeno le units funzionano bene dai. ora è solo un problema di geometria, che a quanto pare sto canando di brutto per qualche ragione. 
	- **ILLUMINAZIONE GENIALE** : ho invertito le coordinate dell'arcotangente ... morto dal ridere
- e riproviamo ... 
	- **ISSUE** tutti i feedback manager sono UNKNOWN ... devo essermi dimenticato di regolarli
	- *e invece no* ... ok, questo è assurdo
- **ISSUE** il robot si avvicina ... ma non abbastanza a quanto pare
	- anzitutto voglio vedere se il sistema funziona, quindi giochicchiamo con l'oracolo
	- il sistema di invio funzionare funziona
	- non mi piace fare modifiche all'oracolo, anche se piccola non mi piace ... perciò vorrei trovare un sistema alternativo per non dover stravoltere tutto l'URDF e nel contempo farlo funzionare
	- posso provare a far venire il robot più vicino al marker, dopotutto ad ogni avvicinamento ho notato che è raro che il robot arrivi esattamente al target: rimane sempre un po' più indietro rispetto all'allineamento tra il punto di partenza e il target
	- (se riuscissi a raggiungere la threshold del prof *senza barare* sarei felicissimo)
- in un futuro auspicabilmente non troppo lontano poi vorrei capire questo da moveit:
	```text
	[ INFO] [1660412162.088435300, 130.624000000]: Completed trajectory execution with status ABORTED ...
	[ INFO] [1660412162.091503800, 130.627000000]: ABORTED: Solution found but controller failed during execution
	```
- **LIMITAZIONE** 3.14 è ingegneristicamente parlando *zero radianti ...*
- **LIMITAZIONE** penso che il wall following in questo contesto sia praticamente inutile. dovrei implementare un controller che non includa il wall follower, solo il go to point
- **LIMITAZIONE** si attiva il wall follow perchè *il sensore laser va a colpire le ruote!*
- ehi ha funzionato! *male, ma è già qualcosa*
- **LIMITAZIONE** il modello del robot fa proprio ... schifo, e anche il controllo non è proprio tutto questo gran chè. l'architettura funziona bene, ma fatica veramente troppo ad andare avanti
	- ci sono volte in cui il robot va contro il muro di lungo, si incastra e non riesce ad uscirne. posso pensare a qualche sistema per evitare che questo accada? oppure ... forse meglio modificare il modello?
- il robot funziona *leggermente meglio* se evito di andare troppo vicino al marker, al 90% ci sta, si può fare
- **COME LOGICA IL ROBOT FUNZIONA**
- **COMMIT**: "testing (very near to the final tests)"

---

posso farmi venire un'idea per la navigazione... dopotutto ho i sensori laser a disposizione: quanto meno posso comandare al robot di "non avvicinarsi troppo" as un ostacolo, anche se però questo ha comunque i suoi rischi. Dovrei provare a sperimentare col sensore laser: l'idea sarebbe di creare un altro behaviour, che *prenda il controllo della situazione* (sto pensando ad un servizio) e allontani il robot da eventuali situazioni tipo quella in cui rimane incastrato. Sinceramente avrei preferito *evitare* a questo punto di fare altri lavori di implementazione ... ma alla fine, se posso migliorare il progetto senza stravolgerlo troppo, va bene così. 

brain storming:

- il problema prima è sort quando il robot è arrivato *troppo vicino* al muro: si è inchiodato sul muro, e ha iniziato a premerci sopra per girare, causando un allineamento completo contro il muro che poi lo ha fatto incastrare
- *se il robot avesse guardato in quel momento il sensore laser* avrebbe capito che la cosa migliore sarebbe stata *andare in retromarcia per guadagnare spazio di manovra*
- il wall following è sicuramente da evitare in questo contesto
- il robot deve inoltre capire quando sta perdendo la posizione. capita spesso che, durante la rotazione, il robot tenda ad allontanarsi dalla posizione iniziale. se durante questa anomalia il robot si fosse segnato la posizione iniziale, e avesse continuamente controllato la distanza dal target, questo non sarebbe successo perchè il robot avrebbe capito che stava perdendo controllabilità, si sarebbe fermato e avrebbe ripreso a muoversi
- sicuramente un tuning migliore dei controlli serve, e serve anche secondo me *fissare delle velocità precise*, discretizzare le velocità piuttosto che mantenerle continue
- il modello sicuramente si può migliorare. avvicinando le ruote ad esempio si riuscirebbe a fare meno forza durante le fasi di rotazione

e iniziamo a lavorarci su

- anzitutto, le ruote (sperando che questo già da solo migliori qualcosa)
	- un piccolo test per capire se effettivamente c'è miglioramento
	- INCREDIBILE! HA FUNZIONATO! stavo già iniziando a temere il peggio
- **LIMITAZIONE** c'è ancora qualche problema quando si tenta di raggiungere un'orientazione di 3.14, non saprei perchè, però per le altre orientazioni tutto funziona come dovrebbe
- a questo punto, potrei anche solo aggiungere una nuova azione all'attuale go_to_point per indietreggiare piuttosto che rifare da capo l'intero algoritmo di motion planning, MOLTO MEGLIO
	- aggiunta dello stato 
	- *ma go to point non ha il laser...*
	- (una soluzione anche solo vagamente decente? *e no, non voglio implementare un nuovo behaviour per una cosa così stupida*)
	- la cosa migliore è usare bug_m , anche se come soluzione è un po' sporca ... *ma va bene lo stesso*
	- e allora, implementazione in bug_m sia!
	- prova ... e ho fatto un casino, sotili incubi logici in cui solo io riesco ad infilarmi
	- dopo essermi fatto venire l'emicrania per un po', *ora funziona*
- in realtà, guardando il risultato che ho ottenuto con poche semplici mosse, mi sembra veramente intuile stare a reimplementare l'universo. un grosso, grasso ,*va bene così*.
- **COMMIT** : "working on robot model and navigation (stability and behavioural improvements)"

---

direi che siamo arrivati all'ultimo grande test: *il mission manager*, e tutta l'ensemble allegramente riunita

- prima di procedere però, voglio provare ancora la ricezione degli hints dall'oracolo, *stavolta usando il raggio di 0.25 originale del nodo.* se c'ho visto bene, con le migliorie che ho fatto funziona anche così. 
- **ISSUE** manca il riferimento al centro nella unit... risolto
- ancora una prova... 
	- devo provare a farlo avvicinare di più il robot, sperando di non cadere in comportamenti instabili come negli esempi precedenti
- **ISSUE** ancora un problema col landmark 2: il robot non va in centro, ma si limita ad andare verso unwaypoint a caso, dopodichè il programma termina, *e non so perchè*
	- (il sonno sta iniziando ad avere la meglio) **ma se non compili ciccio, ma come pretendi che il sistema prenda la modifica?**
	- capito, compilo e riproviamo
- **LIMITAZIONE** ci sono marker alti, e marker bassi ... bisogna distinguere tra i due tipi
- risolviamo qualche limitazione
	- più vicino ai markers! passiamo ad un 97.5% di vicinanza al marker
	- e distinzione tra marker alti e marker bassi
	- *ma prima devo definire un subscriber del tutto analogo a quello della nav unit ...* 
	- *e inoltre ... ho appena scoperto che rosplan non mi invia il marker ...*
	- proviamo a compilare .. a spranghe, compila
	- voglio rivedere velocemente le poses nel pacchetto moveit, alzare la high e alzare leggermente la low
- e test ... 
	- ogni tanto si attiva il wall follow a caso ... devo eliminarlo, altrimenti non se ne esce più
	- e infatti senza il wall follower il robot va molto meglio. ancora non sono convintissimo della macchina a stati, ma va bene così per il momento
- (mi sarei aspettato di fare prima)
- per il momento, l'aiutino sull'oracolo teniamolo: 0.25 è davvero troppo stretto. 
	- dovrei rifare il manipulation controller ... poi, un altro giorno
- **COMMIT** : "working on navigation (solving issued and limitations)"
- e ovviamnete cosa manca? un bel launch di aRMOR ... bello
- a questo punto passiamo direttamente al test completo: il test parziale è un casino, e poi tanto prima o poi avrei dovuto farlo
	- iniziato bene ... poi il programma è letteralmente inciampato su se stesso. era altamente improbabile che andasse bene a primo colpo. 

- *domani ci si pensa*, adesso a mezzanotte mi si incrociano gli occhi.

- **COMMIT**: "first complete test (not working)"

---

## 14/08/2022 -- prova del sistema completo

debug del mission manager

- (andiamo per passi: uno stato alla volta, verificando che tutto proceda bene)
- inserisco una funzione che attenda l'input utente da console, una sorta di "breakpoint"
	- vedi [stackoverflow](https://stackoverflow.com/questions/21257544/c-wait-for-user-input) (ho cercato, magari esisteva un metodo più furbo ... ma a quanto pare non è così)
	- la macro:
		```c++
		#define WAITKEY_ENABLED true
		#define WAITKEY { if( WAITKEY_ENABLED ) { std::cout << "press ENTER to continue ... " ; sstd::cin.get( ) ; std::cout << std::endl ; } }
		```
	- ne va messa una ad ogni ingresso di fase della missione, e una ad ogni statement `continue` o `return`. 
	- compila? direi di sì (ma bisogna vedere se funge)
- e passiamo a testare
	- il primissimo REPLAN funziona
	-MISSION_STATUS_COLLECT per qualche ragione lancia REPLAN invece che COLLECT ... e dal codice non sembra affatto chiaro quale sia il problema, dato che il landmark viene settato correttamente
		- **BECCATO!** nella `make_plan()` usavo come landmark LA COSTANTE piuttosto che il valore del parametro ... boh
	- **ISSUE** per qualche ragione non riceve lo shutdown il nodo. dopo aver fatto ctrl+C il nodo rimane in "stopping..." ma non si chiude
	- la COLLECT funge
	- e la ASK pure a quanto pare
- stava andando tutto bene, fin quando il robot non è arrivato al quarto landmark:

```text
root@3b17871017fd:~/ros_ws/src/erl2-new# rosrun robocluedo_mission_manager mission_manager
[ INFO] [1660460418.195116500]: [robocluedo_mission_manager] starting ... 
[ INFO] [1660460418.196407600]: [robocluedo_mission_manager] Opening client [/robocluedo/pipeline_manager] ...
[ INFO] [1660460418.197618500]: [robocluedo_mission_manager] Opening client [/robocluedo/pipeline_manager] ... OK
[ INFO] [1660460418.197674800]: [robocluedo_mission_manager] Opening client [/cluedo_armor/add_hint] ...
[ INFO] [1660460418.198233900]: [robocluedo_mission_manager] Opening client [/cluedo_armor/add_hint] ... OK
[ INFO] [1660460418.198291300]: [robocluedo_mission_manager] Opening client [/cluedo_armor/find_consistent_h] ...
[ INFO] [1660460418.198825800]: [robocluedo_mission_manager] Opening client [/cluedo_armor/find_consistent_h] ... OK
[ INFO] [1660460418.199101100]: [robocluedo_mission_manager] Opening client [/cluedo_armor/wrong_hypothesis] ...
[ INFO] [1660460418.199661000]: [robocluedo_mission_manager] Opening client [/cluedo_armor/wrong_hypothesis] ... OK
[ INFO] [1660460418.199908500]: [robocluedo_mission_manager] Opening client [/cluedo_armor/backup] ...
[ INFO] [1660460418.200465800]: [robocluedo_mission_manager] Opening client [/cluedo_armor/backup] ... OK
[ INFO] [1660460418.200778500]: [robocluedo_mission_manager] Opening client [/oracle_solution] ...
[ INFO] [1660460418.201464000]: [robocluedo_mission_manager] Opening client [/oracle_solution] ... OK
[ INFO] [1660460418.201500800]: [robocluedo_mission_manager] subscribing to /oracle_hint ... 
[ INFO] [1660460418.204203900]: [robocluedo_mission_manager] subscribing to /oracle_hint ... OK
[ INFO] [1660460418.228821100]: [robocluedo_mission_manager] ready
[ INFO] [1660460418.228863900]: [robocluedo_mission_manager] mission manager STARTING WORKING CYCLE
press ENTER to continue ... 

[ INFO] [1660460420.256930200, 145.951000000]: [robocluedo_mission_manager] status: MISSION_STATUS_REPLAN
press ENTER to continue ... 

press ENTER to continue ... 

[ INFO] [1660460432.031566100, 155.948000000]: [robocluedo_mission_manager] status: MISSION_STATUS_COLLECT
press ENTER to continue ... 

[ INFO] [1660460462.566825900, 181.214000000]: [robocluedo_mission_manager] (cbk_oracle_hint, add hint) received hint with content (ID=0, key=who, value=missScarlett)
[ INFO] [1660460462.846654800, 181.432000000]: [robocluedo_mission_manager] (cbk_oracle_hint, add hint) valid hint received and registered
press ENTER to continue ... 

[ INFO] [1660460493.867900700, 207.520000000]: [robocluedo_mission_manager] status: MISSION_STATUS_ASK_ONTOLOGY
press ENTER to continue ... 

[ INFO] [1660460507.569821400, 219.268000000]: [robocluedo_mission_manager] no brilliant ideas from the RoboCLuedo B.B.B. (Big Brain Bruh) COLLECTING AGAIN
[ INFO] [1660460507.569901300, 219.268000000]: [robocluedo_mission_manager] status: MISSION_STATUS_COLLECT
press ENTER to continue ... 

[ INFO] [1660460551.924768000, 256.869000000]: [robocluedo_mission_manager] (cbk_oracle_hint, add hint) received hint with content (ID=3, key=what, value=rope)
[ INFO] [1660460552.174746000, 257.041000000]: [robocluedo_mission_manager] (cbk_oracle_hint, add hint) valid hint received and registered


press ENTER to continue ... 
[ INFO] [1660460577.442745800, 278.420000000]: [robocluedo_mission_manager] status: MISSION_STATUS_ASK_ONTOLOGY
press ENTER to continue ... 
[ INFO] [1660460577.524677600, 278.500000000]: [robocluedo_mission_manager] no brilliant ideas from the RoboCLuedo B.B.B. (Big Brain Bruh) COLLECTING AGAIN
[ INFO] [1660460577.524750600, 278.500000000]: [robocluedo_mission_manager] status: MISSION_STATUS_COLLECT
press ENTER to continue ... 

[ INFO] [1660460614.284841800, 309.496000000]: [robocluedo_mission_manager] (cbk_oracle_hint, add hint) received hint with content (ID=3, key=what, value=leadPipe)
[ INFO] [1660460614.542821400, 309.706000000]: [robocluedo_mission_manager] (cbk_oracle_hint, add hint) valid hint received and registered
press ENTER to continue ... 

[ INFO] [1660460644.022687800, 334.679000000]: [robocluedo_mission_manager] status: MISSION_STATUS_ASK_ONTOLOGY
press ENTER to continue ... 

[ INFO] [1660460646.470102900, 336.749000000]: [robocluedo_mission_manager] no brilliant ideas from the RoboCLuedo B.B.B. (Big Brain Bruh) COLLECTING AGAIN
[ INFO] [1660460646.470179600, 336.749000000]: [robocluedo_mission_manager] status: MISSION_STATUS_COLLECT
press ENTER to continue ... 

[ INFO] [1660460689.925096000, 373.548000000]: [robocluedo_mission_manager] (cbk_oracle_hint, add hint) received hint with content (ID=4, key=who, value=-1)
[ INFO] [1660460689.925142300, 373.548000000]: [robocluedo_mission_manager] (cbk_oracle_hint, add hint) NOT VALID hint, skip
press ENTER to continue ... 

[ INFO] [1660460717.820632200, 397.160000000]: [robocluedo_mission_manager] status: MISSION_STATUS_ASK_ONTOLOGY
press ENTER to continue ... 

[ INFO] [1660460719.465309900, 398.545000000]: [robocluedo_mission_manager] no brilliant ideas from the RoboCLuedo B.B.B. (Big Brain Bruh) COLLECTING AGAIN
[ INFO] [1660460719.465353000, 398.545000000]: [robocluedo_mission_manager] status: MISSION_STATUS_COLLECT
press ENTER to continue ... 

[ WARN] [1660460724.006679600, 402.454000000]: [robocluedo_mission_manager] WARNING: unable to load the problem! RETRYING
press ENTER to continue ... 

[ WARN] [1660460741.215243700, 417.067000000]: [robocluedo_mission_manager] WARNING: status: MISSION_STATUS_COUNT_FAULT (remaining 9)
press ENTER to continue ... 

[ INFO] [1660460743.781805500, 419.283000000]: [robocluedo_mission_manager] status: MISSION_STATUS_COLLECT
press ENTER to continue ... 

[ WARN] [1660460748.547209900, 423.408000000]: [robocluedo_mission_manager] WARNING: unable to load the problem! RETRYING
press ENTER to continue ... 

[ WARN] [1660460778.918767200, 449.660000000]: [robocluedo_mission_manager] WARNING: status: MISSION_STATUS_COUNT_FAULT (remaining 8)
press ENTER to continue ... 

[ INFO] [1660460784.046885600, 454.064000000]: [robocluedo_mission_manager] status: MISSION_STATUS_COLLECT
press ENTER to continue ... 

[ WARN] [1660460786.128425500, 455.724000000]: [robocluedo_mission_manager] WARNING: unable to load the problem! RETRYING
press ENTER to continue ... 
```

- c'è un output molto strano dal pipeline manager in questo caso:

```text
[INFO] [1660460723.687263, 402.195000]: (rosplan_pipeline_manager) problem interface -- loading problem ...
<class 'int'>
[ INFO] [1660460723.912317100, 402.372000000]: KCL: (/rosplan_knowledge_base) Removing goal (hint-collected)
[ INFO] [1660460723.923327800, 402.385000000]: KCL: (/rosplan_knowledge_base) Removing goal (at)
[INFO] [1660460748.265953, 423.156000]: (rosplan_pipeline_manager) problem interface -- loading problem ...
<class 'int'>
[INFO] [1660460785.780212, 455.469000]: (rosplan_pipeline_manager) problem interface -- loading problem ...
<class 'int'>

```

- (che python abbia colpito ancora?)
- il problema è che al mission manager arriva false al loading, il che mi sembra piuttosto strano perchè nulla cancella il file da caricare, e inoltre la lettura ha funzionato bene finora, quindi mi pare piuttosto improbabile che proprio quello non funzioni. 
- penso più ad un problema di flags: il pipeline manager invia false quando dovrebbe inviare true sul load e false sul solve
	- un'occhiata al pipeline manager...
	- guardando il codice e guardando l'output mi pare stranissimo che il codice si pianti a quel punto ... tempo che ci sia un altro fastidiosissimo errore Python alla callback, nascosto da `2>/dev/null`
	- devo runnare rosplan in una bash a parte per poter osservare questo errore
- **BECCATO!** l'errore sta nella outcome al verificarsi di questa situazione

outout ROSPlan: (vedi in particolare l'ultima riga)

```text
[ INFO] [1660461899.384484700, 368.281000000]: KCL: (/rosplan_plan_dispatcher) Action [2] is 1660461558.747440 second(s) late
[ INFO] [1660461899.387802700, 368.281000000]: KCL: (collect-hint) action received
[ INFO] [1660461899.395119400, 368.290000000]: KCL: (/rosplan_knowledge_base) Removing Fact (not-hint-collected,0)
[ INFO] [1660461899.395371600, 368.290000000]: KCL: (/rosplan_knowledge_base) Removing Fact (hint-ready,0)
[ INFO] [1660461899.395691800, 368.289000000]: [collect_hint] (collect-hint ?wp4)
[ INFO] [1660461899.395955200, 368.289000000]: [collect_hint] manipulator PHASE 1
[ INFO] [1660461899.486790600, 368.360000000]: KCL: (/rosplan_plan_dispatcher) Feedback received [2, 1]
[ INFO] [1660461899.486843400, 368.360000000]: KCL: (/rosplan_plan_dispatcher) Feedback received [2, 1]
[ INFO] [1660461909.056033800, 376.331000000]: [collect_hint] manipulator WAITING
[ INFO] [1660461911.484642900, 378.333000000]: [collect_hint] manipulator PHASE 2
[ INFO] [1660461920.840904400, 386.340000000]: KCL: (collect-hint) action completed successfully
[ INFO] [1660461920.850418500, 386.352000000]: KCL: (/rosplan_knowledge_base) Adding fact (hint-collected wp4, 0)
[ INFO] [1660461920.850520100, 386.352000000]: KCL: (/rosplan_knowledge_base) Adding fact (not-hint-ready, 0)
[ INFO] [1660461920.852806600, 386.356000000]: KCL: (/rosplan_plan_dispatcher) Feedback received [2, 2]
[ INFO] [1660461920.993050300, 386.455000000]: KCL: (/rosplan_plan_dispatcher) Dispatch complete.
[INFO] [1660461927.192263, 391.719000]: (rosplan_pipeline_manager) problem interface -- loading problem ...
[ INFO] [1660461927.382111400, 391.884000000]: KCL: (/rosplan_knowledge_base) Removing goal (hint-collected)
[ INFO] [1660461927.398705300, 391.894000000]: KCL: (/rosplan_knowledge_base) Removing goal (at)
[WARN] [1660461927.484973, 391.963000]: (rosplan_pipeline_manager) unable to apply the landmark 1 (reason: landmark not applicable)
```

output mission manager:

```text
[ INFO] [1660461854.394579400, 330.383000000]: [robocluedo_mission_manager] status: MISSION_STATUS_ASK_ONTOLOGY
press ENTER to continue ... 

[ INFO] [1660461856.196549900, 331.908000000]: [robocluedo_mission_manager] no brilliant ideas from the RoboCLuedo B.B.B. (Big Brain Bruh) COLLECTING AGAIN
[ INFO] [1660461856.196642700, 331.908000000]: [robocluedo_mission_manager] status: MISSION_STATUS_COLLECT
press ENTER to continue ... 

[ INFO] [1660461896.403702500, 365.783000000]: [robocluedo_mission_manager] (cbk_oracle_hint, add hint) received hint with content (ID=2, key=who, value=missScarlett)
[ INFO] [1660461896.560204400, 365.880000000]: [robocluedo_mission_manager] (cbk_oracle_hint, add hint) valid hint received and registered
press ENTER to continue ... 

[ INFO] [1660461924.169541900, 389.179000000]: [robocluedo_mission_manager] status: MISSION_STATUS_ASK_ONTOLOGY
press ENTER to continue ... 

[ INFO] [1660461925.436996600, 390.256000000]: [robocluedo_mission_manager] no brilliant ideas from the RoboCLuedo B.B.B. (Big Brain Bruh) COLLECTING AGAIN
[ INFO] [1660461925.437071300, 390.256000000]: [robocluedo_mission_manager] status: MISSION_STATUS_COLLECT
press ENTER to continue ... 

[ WARN] [1660461927.486965400, 391.966000000]: [robocluedo_mission_manager] WARNING: unable to load the problem! RETRYING
press ENTER to continue ... 

[ WARN] [1660462017.573408400, 468.375000000]: [robocluedo_mission_manager] WARNING: status: MISSION_STATUS_COUNT_FAULT (remaining 9)
press ENTER to continue ... 

[ INFO] [1660462019.221448000, 469.797000000]: [robocluedo_mission_manager] status: MISSION_STATUS_COLLECT
press ENTER to continue ... 

[ WARN] [1660462021.992463400, 472.069000000]: [robocluedo_mission_manager] WARNING: unable to load the problem! RETRYING
press ENTER to continue ... 

```

- questo significa che fortunatamente il problema sta nel mission manager (quindi è facile da risolvere ... fiuuuu ... )
- c'è una particolarità nel servizio rosplan pipeline manager: quando il landmark non è applicabile, il pipeline manager *non chiama comunque il planner*, ma dopo aver approvato la non applicabilità del landmark, ritorna che la load non può essere eseguita. quindi, nel landmark 1, questo non è un caso di errore, ma un caso che semplicemente modifica lo stato
	- *una domanda fastidiosa* : ma se quel false fosse riferito invece ad una *failure* del servizio? può succedere dopotutto
		- *bella domanda...*
	- una soluzione può essere quella di aggiungere un flag che permetta di distinguere la non applicabilità del landmark dalla failure vera e propria
		- aggiunta del campo al servizio
		- *modifica al pipeline manager* (occhio.........)
		- e modifica al mission manager per supportare il nuovo flag
		- compilare compila
- e si prova di nuovo
	- voglio provare a togliere i waitkey e a vedere come va "a briglie sciolte"
	- *per il futuro* sarebbe meglio fare una modalità di debug che funzioni da parameter server piuttosto che definire dei parametri e dover ricompilare tutto ogni volta
	- **E UNA E' ANDATA!** 

- ora, c'è un problema con le ipotesi consistenti.

```
root@3b17871017fd:~/ros_ws/src/erl2-new# rosrun robocluedo_mission_manager mission_manager
[ INFO] [1660464813.088098700]: [robocluedo_mission_manager] starting ... 
[ INFO] [1660464813.090170500]: [robocluedo_mission_manager] Opening client [/robocluedo/pipeline_manager] ...
[ INFO] [1660464813.091644400]: [robocluedo_mission_manager] Opening client [/robocluedo/pipeline_manager] ... OK
[ INFO] [1660464813.092038000]: [robocluedo_mission_manager] Opening client [/cluedo_armor/add_hint] ...
[ INFO] [1660464813.094735800]: [robocluedo_mission_manager] Opening client [/cluedo_armor/add_hint] ... OK
[ INFO] [1660464813.095365700]: [robocluedo_mission_manager] Opening client [/cluedo_armor/find_consistent_h] ...
[ INFO] [1660464813.097256000]: [robocluedo_mission_manager] Opening client [/cluedo_armor/find_consistent_h] ... OK
[ INFO] [1660464813.097323300]: [robocluedo_mission_manager] Opening client [/cluedo_armor/wrong_hypothesis] ...
[ INFO] [1660464813.098976400]: [robocluedo_mission_manager] Opening client [/cluedo_armor/wrong_hypothesis] ... OK
[ INFO] [1660464813.099051400]: [robocluedo_mission_manager] Opening client [/cluedo_armor/backup] ...
[ INFO] [1660464813.100113600]: [robocluedo_mission_manager] Opening client [/cluedo_armor/backup] ... OK
[ INFO] [1660464813.100198100]: [robocluedo_mission_manager] Opening client [/oracle_solution] ...
[ INFO] [1660464813.104764600]: [robocluedo_mission_manager] Opening client [/oracle_solution] ... OK
[ INFO] [1660464813.104856800]: [robocluedo_mission_manager] subscribing to /oracle_hint ... 
[ INFO] [1660464813.107836100]: [robocluedo_mission_manager] subscribing to /oracle_hint ... OK
[ INFO] [1660464813.107976100]: [robocluedo_mission_manager] ready
[ INFO] [1660464813.108064500]: [robocluedo_mission_manager] mission manager STARTING WORKING CYCLE

[ INFO] [1660464813.108126700]: [robocluedo_mission_manager] status: MISSION_STATUS_REPLAN

[ INFO] [1660464819.133991300, 144.109000000]: [robocluedo_mission_manager] status: MISSION_STATUS_COLLECT
[ INFO] [1660464882.251502800, 197.926000000]: [robocluedo_mission_manager] (cbk_oracle_hint, add hint) received hint with content (ID=1, key=, value=revolver)
[ INFO] [1660464882.251554600, 197.926000000]: [robocluedo_mission_manager] (cbk_oracle_hint, add hint) NOT VALID hint, skip
[ INFO] [1660464906.749652000, 219.549000000]: [robocluedo_mission_manager] status: MISSION_STATUS_ASK_ONTOLOGY
[ INFO] [1660464906.875105900, 219.635000000]: [robocluedo_mission_manager] no brilliant ideas from the RoboCLuedo B.B.B. (Big Brain Bruh) COLLECTING AGAIN

[ INFO] [1660464906.875148700, 219.635000000]: [robocluedo_mission_manager] status: MISSION_STATUS_COLLECT
[ INFO] [1660464940.332236900, 248.285000000]: [robocluedo_mission_manager] (cbk_oracle_hint, add hint) received hint with content (ID=2, key=what, value=rope)
[ INFO] [1660464940.666278200, 248.518000000]: [robocluedo_mission_manager] (cbk_oracle_hint, add hint) valid hint received and registered
[ INFO] [1660464965.108541000, 269.932000000]: [robocluedo_mission_manager] status: MISSION_STATUS_ASK_ONTOLOGY
[ INFO] [1660464965.190347900, 269.992000000]: [robocluedo_mission_manager] no brilliant ideas from the RoboCLuedo B.B.B. (Big Brain Bruh) COLLECTING AGAIN

[ INFO] [1660464965.190460400, 269.992000000]: [robocluedo_mission_manager] status: MISSION_STATUS_COLLECT
[ INFO] [1660464993.823901000, 294.687000000]: [robocluedo_mission_manager] (cbk_oracle_hint, add hint) received hint with content (ID=5, key=who, value=mrsWhite)
[ INFO] [1660464994.028262600, 294.833000000]: [robocluedo_mission_manager] (cbk_oracle_hint, add hint) valid hint received and registered
[ INFO] [1660465021.162767200, 318.449000000]: [robocluedo_mission_manager] status: MISSION_STATUS_ASK_ONTOLOGY
[ INFO] [1660465021.240666700, 318.513000000]: [robocluedo_mission_manager] no brilliant ideas from the RoboCLuedo B.B.B. (Big Brain Bruh) COLLECTING AGAIN

[ INFO] [1660465021.240766100, 318.513000000]: [robocluedo_mission_manager] status: MISSION_STATUS_COLLECT
[ INFO] [1660465057.858384000, 349.682000000]: [robocluedo_mission_manager] (cbk_oracle_hint, add hint) received hint with content (ID=5, key=what, value=revolver)
[ INFO] [1660465058.093437000, 349.853000000]: [robocluedo_mission_manager] (cbk_oracle_hint, add hint) valid hint received and registered
[ INFO] [1660465082.987075500, 371.376000000]: [robocluedo_mission_manager] status: MISSION_STATUS_ASK_ONTOLOGY
[ INFO] [1660465083.025977500, 371.416000000]: [robocluedo_mission_manager] no brilliant ideas from the RoboCLuedo B.B.B. (Big Brain Bruh) COLLECTING AGAIN

[ INFO] [1660465083.026057500, 371.416000000]: [robocluedo_mission_manager] status: MISSION_STATUS_COLLECT
[ WARN] [1660465083.370297200, 371.676000000]: [robocluedo_mission_manager] WARNING: landmark COLLECT no longer applicable (all the waypoints have been explored) TRYING TO directly SOLVE THE MYSTERY

[ INFO] [1660465083.370458800, 371.678000000]: [robocluedo_mission_manager] status: MISSION_STATUS_ASK_ONTOLOGY
[ INFO] [1660465083.451198800, 371.717000000]: [robocluedo_mission_manager] no brilliant ideas from the RoboCLuedo B.B.B. (Big Brain Bruh) REPLANNING

[ INFO] [1660465083.451276500, 371.717000000]: [robocluedo_mission_manager] status: MISSION_STATUS_REPLAN

[ INFO] [1660465089.413400700, 376.855000000]: [robocluedo_mission_manager] status: MISSION_STATUS_COLLECT
[ INFO] [1660465147.673380700, 426.049000000]: [robocluedo_mission_manager] (cbk_oracle_hint, add hint) received hint with content (ID=5, key=when, value=-1)
[ INFO] [1660465147.673434500, 426.049000000]: [robocluedo_mission_manager] (cbk_oracle_hint, add hint) NOT VALID hint, skip
[ INFO] [1660465172.558713000, 447.613000000]: [robocluedo_mission_manager] status: MISSION_STATUS_ASK_ONTOLOGY
[ INFO] [1660465172.659145500, 447.675000000]: [robocluedo_mission_manager] no brilliant ideas from the RoboCLuedo B.B.B. (Big Brain Bruh) COLLECTING AGAIN

[ INFO] [1660465172.659197000, 447.675000000]: [robocluedo_mission_manager] status: MISSION_STATUS_COLLECT
[ INFO] [1660465206.605444200, 476.502000000]: [robocluedo_mission_manager] (cbk_oracle_hint, add hint) received hint with content (ID=5, key=where, value=bathroom)
[ INFO] [1660465206.803322900, 476.625000000]: [robocluedo_mission_manager] (cbk_oracle_hint, add hint) valid hint received and registered
[ INFO] [1660465231.859765100, 498.152000000]: [robocluedo_mission_manager] status: MISSION_STATUS_ASK_ONTOLOGY


[ INFO] [1660465231.957811700, 498.233000000]: [robocluedo_mission_manager] (from the ontology) found 1consistent hypotheses


[ INFO] [1660465231.957864000, 498.233000000]: [robocluedo_mission_manager] status: MISSION_STATUS_SOLVE
[ WARN] [1660465233.820674500, 499.839000000]: [robocluedo_mission_manager] WARNING: UNEXPECTED! Plan not solvable, can't find a solution (maybe a issue with the PDDL model?)
[ WARN] [1660465233.820753800, 499.839000000]: [robocluedo_mission_manager] WARNING: status: MISSION_STATUS_COUNT_FAULT (remaining 9)


[ INFO] [1660465233.820833900, 499.839000000]: [robocluedo_mission_manager] status: MISSION_STATUS_REPLAN
[ INFO] [1660465239.883059400, 504.990000000]: [robocluedo_mission_manager] status: MISSION_STATUS_COLLECT
^C[ INFO] [1660465358.404594600, 604.827000000]: [robocluedo_mission_manager] stopping... 
root@3b17871017fd:~/ros_ws/src/erl2-new# 

```

- **ASPETTA UN ATTIMO! CHE COSA SAREBBE `when`???**
	- maaaaa non l'avevo visto! non avevo notato il caso 4!

		```c++
		if(a==0){
			oracle_msg.key = "";
			oracle_msg.value = "";
		}
		if (a==1){
			oracle_msg.key="";
			oracle_msg.value=person[rand()%6];
		}
		if (a==2){
			oracle_msg.key="";
			oracle_msg.value=object[rand()%6];
		}
		if (a==3){
			oracle_msg.key="when";
			oracle_msg.value="-1";
		}
		if (a==4){
			oracle_msg.key="who";
			oracle_msg.value="-1";
		}
		```
	
	- **ISSUE** occorre aggiungere un altro controllo di validità: la key dev'essere *esattamente* una tra who, where e what
	- ok niente, incidentalmente avevo già pensato a questa eventualità, vedi implementazione attuale di add hint. anche fosse passato, non sarebbe stato accettato da aRMOR
		
		```c++
		if( hint.property == "where" )
			armor->AddIndiv( hint.Belem, "PLACE" );
		else if( hint.property == "who" )
			armor->AddIndiv( hint.Belem, "PERSON" );
		else if( hint.property == "what" )
			armor->AddIndiv( hint.Belem, "WEAPON" );
		else
		{
			OUTLOG( "ERROR: not a valid hint property (received " << hint.property << ")" );
			
			success.success = false;
			return true;
		}
		```

- devo capire adesso perchè a quel punto il plan non è risolvibile. pare esserci un problema quando il robot richiama la move to center per la seconda volta
	- potrebbe essere un disguido con le preconditions di move to center?
	- **BECCATO!** ricordo che nell'esecuzione di prima (dannato me che ho chiuso la console senza annotare...) il robot è passato dal centro, quindi il centro era come se fosse stato già esplorato
	- in quel caso, *bisogna richiamare il replan* e poi procedere con la soluzione
	- riguardando il PDDL Potrei aver notato una falla grande quanto una casa nella parte del solve ... il predicato `(not-hint-ready )` viene usato malissimo, in maniera proprio insensata. e nell'output del planner questi manca, così come il suo opposto `(hint-ready )`
		- nel problema iniziale c'è, quindi la kb interface evidentemente lo cancella o non tiene conto della sua presenza come dovrebbe, vero? ... 
		- ... *mi sbagliavo.*
	- **ISSUE** però avevo ragione su `(not-hint-ready )`, non mi piace come viene usato, e se c'ho visto bvene può anche essere eliminato e basta

- il problema con la solve è di natura *topologica*.
	- attualmente la mappa è costituito da un unico percorso (per ragioni di semplicità) centro -> wp1 -> wp1 -> wp3 -> wp4 <-> centro
	- ogni volta che il robot usa la move-to verso un wp, il wp di arrivo viene marcato come explored
	- nel caso precedente, il robot è partito da wp4 anzichè dal centro, ha chiamato la move-to-center, e poi la move-to su wp1 e poi su wp2 fin quando non ha trovato un'ipotesi consistente
	- il sistema allora ha chiamato il landmark SOLVE per muovere il robot al centro
	- qui il planner non è stato in grado di trovare una soluzione: non poteva chiamare move-to-center perchè non esiste un percorso che leghi wp2 al centro, quindi bisogna passare per altri punti, **e la knowledge base interface mantiene il flag explored** quindi wp4 risultava *explored*, e la move-to non consente di passare da un punto explored! **ecco perchè plan unsolvable**
- il problema si può risolvere facilmente facendo replanning prima di richiamare il solve, facile se hai un flag "readu_to_solve" nel codice del mission manager
	- trovo sia più che lecito che la kb interface tratti in questa maniera i waypoints, altrimenti il robot ripeterebbe lo stesso movimento
	- quello che non capisco però, e che non mi va bene, è che *la kb interface avrebbe dovuto controllare l'applicabilità dell'azione*, e il pipeline manager avrebbe dovuto controllare che il landmark fosse applicabile
		- infatti non c'è alcun check nella kb interface a riguardo
	- altra domanda spinosa, *e come lo implementerei questo check?* 
		- temo non ci sia alcun modo facile a dire il vero... cosa dovrei fare? verifico che esista un percorso tra wp2 e il centro? mi sembra assurdo, e complicherebbe inutilmente un nodo che già è un prodotto altamente custom nonostante gli sforzi per renderlo vagamanete generale
	- quindi *la soluzione migliore* (mio modesto parere) *è di cambiare reazione alla plan unsolvable* ordinando un replan

- quindi,
	- modifica mission manager per supportare il replan intermedio

- **LIMITAZIONE** bisognerebbe ristrutturare leggermente la kb interface in modo tale che riesca a gestire vari aspetti distinti del problema: ad esempio la topologia, distinta da la localizzazione, e tutto il resto. Se il compito della kb_interface è quello di *fornire servizi specializzati* al sistema, allora penso si possa fare molto di più. 
	- un altro difetto potrebbe essere mischiare troppo le cose tra pipeline manager e kb interface. la kb interface sarebbe meglio che fosse un nodo che esponga servizi *anche al di fuori del package*.
	- unico compito del pipeline manager è quello di *gestire il flusso di esecuzione in base ai landmarks*, il che sicuramente coinvolge la kb_interface, *ma sempre nel pieno rispetto del compito iniziale*: la pipeline manager *non dovrebbe offrire servizi diversi dall'esecuzione di uno specifico landmark* e il fornire una prima spiegazione ad eventuali problemi che possono sorgere
	- quando la pipeline manager solleva un problema, la kb_interface dovrebbe mettere a disposizione dei servizi separati per aiutare a comprendere il problema.
	- *attualmente la kb_interface ha solo rilevanza interna, e il pipeline manager fa tutto*, dando luogo a situazioni ambigue come quella che ho appena visto. 

- e si torna a provare
	- ... la prova infinita (senza nulla di insolito) ... meglio riavviare
- **ISSUE** nessun backup dalla ontology! (me lo sono dimenticato onestamente)
- e si prova ancora (sperando stavolta di ottenere un'ipotesi consistente in tempi ragionevoli, così provo tutto il flow una volta e fine)
	- e alla ASK_ORACLE ci siamo arrivati! 
	- però adesso va in ciclo infinito su quello stesso stato ... e conoscendomi so *perfettamente* perchè (dannato me che copio incollo selvaggiamente)
	- totalmente missato lo stato successivo ... bello
- giusto un'occhiata alla ontology dato che ci siamo fermati
	- sembra ben formata: la deduzione era corretta, anche se non era soluzione del caso
- *e proviamo ancora*
	- stesso problema di prima ...
	- **indovina un po' chi ha dimenticato di ricompilare? ARIDANGHETEEEEEE**
- beh, a questo punto faccio una modifica all'oracolo giusto per fare i test più velocemente

- **COMMIT** : "testing (mission manager debugging, erl oracle debug mode)" 

---

eeee ennesiam sessione di test:

- (dopo aver aggiunto la modalità di sviluppo all'oracolo)
- serve documentare nei test
- *e ora*, indaghiamo (sul senso della vita)
	- **COMPILAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA**
	- ho fatto un *mezzo* casino con l'oracolo, ma si risolve facilmente
	- **COMPILAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA**
- visto come vanno le cose posso anche settare direttamente lo SCALING_FACTOR a 0.99

- **EANDIAMO.**

	```text
	root@3b17871017fd:~/ros_ws/src/erl2-new# rosrun robocluedo_mission_manager mission_manager
	[ INFO] [1660477923.310792700]: [robocluedo_mission_manager] starting ... 
	[ INFO] [1660477923.338249500]: [robocluedo_mission_manager] Opening client [/robocluedo/pipeline_manager] ...
	[ INFO] [1660477923.340275700]: waitForService: Service [/robocluedo/pipeline_manager] has not been advertised, waiting...
	[ INFO] [1660477937.913722200, 138.900000000]: waitForService: Service [/robocluedo/pipeline_manager] is now available.
	[ INFO] [1660477937.913775000, 138.900000000]: [robocluedo_mission_manager] Opening client [/robocluedo/pipeline_manager] ... OK
	[ INFO] [1660477937.913804300, 138.900000000]: [robocluedo_mission_manager] Opening client [/cluedo_armor/add_hint] ...
	[ INFO] [1660477937.915489500, 138.901000000]: [robocluedo_mission_manager] Opening client [/cluedo_armor/add_hint] ... OK
	[ INFO] [1660477937.915635100, 138.901000000]: [robocluedo_mission_manager] Opening client [/cluedo_armor/find_consistent_h] ...
	[ INFO] [1660477937.916569100, 138.902000000]: [robocluedo_mission_manager] Opening client [/cluedo_armor/find_consistent_h] ... OK
	[ INFO] [1660477937.916651500, 138.902000000]: [robocluedo_mission_manager] Opening client [/cluedo_armor/wrong_hypothesis] ...
	[ INFO] [1660477937.917376500, 138.902000000]: [robocluedo_mission_manager] Opening client [/cluedo_armor/wrong_hypothesis] ... OK
	[ INFO] [1660477937.917413400, 138.902000000]: [robocluedo_mission_manager] Opening client [/cluedo_armor/backup] ...
	[ INFO] [1660477937.918439600, 138.904000000]: [robocluedo_mission_manager] Opening client [/cluedo_armor/backup] ... OK
	[ INFO] [1660477937.918480900, 138.904000000]: [robocluedo_mission_manager] Opening client [/oracle_solution] ...
	[ INFO] [1660477937.919124000, 138.906000000]: [robocluedo_mission_manager] Opening client [/oracle_solution] ... OK
	[ INFO] [1660477937.919214500, 138.906000000]: [robocluedo_mission_manager] subscribing to /oracle_hint ... 
	[ INFO] [1660477937.922416100, 138.909000000]: [robocluedo_mission_manager] subscribing to /oracle_hint ... OK
	[ INFO] [1660477937.922479500, 138.909000000]: [robocluedo_mission_manager] ready
	[ INFO] [1660477937.922499700, 138.909000000]: [robocluedo_mission_manager] mission manager STARTING WORKING CYCLE
	[ INFO] [1660477937.922518700, 138.909000000]: [robocluedo_mission_manager] status: MISSION_STATUS_REPLAN
	[ INFO] [1660477943.840810200, 143.947000000]: [robocluedo_mission_manager] gotta catch 'em all! NA NA NA NA NA NA
	[ INFO] [1660477943.840866700, 143.947000000]: [robocluedo_mission_manager] status: MISSION_STATUS_COLLECT
	[ INFO] [1660478000.065528700, 191.676000000]: [robocluedo_mission_manager] (cbk_oracle_hint, add hint) received hint with content (ID=0, key=who, value=missScarlett)
	[ INFO] [1660478000.337667300, 191.893000000]: [robocluedo_mission_manager] (cbk_oracle_hint, add hint) valid hint received and registered
	[ INFO] [1660478015.775549300, 205.276000000]: [robocluedo_mission_manager] status: MISSION_STATUS_ASK_ONTOLOGY
	[ INFO] [1660478015.820921200, 205.316000000]: [robocluedo_mission_manager] no brilliant ideas from the RoboCLuedo B.B.B. (Big Brain Bruh) COLLECTING AGAIN
	[ INFO] [1660478015.820994300, 205.316000000]: [robocluedo_mission_manager] status: MISSION_STATUS_COLLECT
	[ INFO] [1660478064.410216300, 246.860000000]: [robocluedo_mission_manager] (cbk_oracle_hint, add hint) received hint with content (ID=0, key=who, value=missScarlett)
	[ INFO] [1660478064.495085900, 246.920000000]: [robocluedo_mission_manager] (cbk_oracle_hint, add hint) valid hint received and registered
	[ INFO] [1660478074.302948400, 255.592000000]: [robocluedo_mission_manager] status: MISSION_STATUS_ASK_ONTOLOGY
	[ INFO] [1660478074.337975300, 255.617000000]: [robocluedo_mission_manager] no brilliant ideas from the RoboCLuedo B.B.B. (Big Brain Bruh) COLLECTING AGAIN
	[ INFO] [1660478074.338025600, 255.617000000]: [robocluedo_mission_manager] status: MISSION_STATUS_COLLECT
	[ INFO] [1660478115.632459800, 291.042000000]: [robocluedo_mission_manager] (cbk_oracle_hint, add hint) received hint with content (ID=0, key=who, value=missScarlett)
	[ INFO] [1660478115.683327800, 291.083000000]: [robocluedo_mission_manager] (cbk_oracle_hint, add hint) valid hint received and registered
	[ INFO] [1660478130.942204600, 304.335000000]: [robocluedo_mission_manager] status: MISSION_STATUS_ASK_ONTOLOGY
	[ INFO] [1660478130.976974500, 304.349000000]: [robocluedo_mission_manager] no brilliant ideas from the RoboCLuedo B.B.B. (Big Brain Bruh) COLLECTING AGAIN
	[ INFO] [1660478130.977089700, 304.349000000]: [robocluedo_mission_manager] status: MISSION_STATUS_COLLECT
	[ INFO] [1660478181.943447500, 348.124000000]: [robocluedo_mission_manager] (cbk_oracle_hint, add hint) received hint with content (ID=0, key=where, value=conservatory)
	[ INFO] [1660478182.066006900, 348.211000000]: [robocluedo_mission_manager] (cbk_oracle_hint, add hint) valid hint received and registered
	[ INFO] [1660478192.309954000, 356.905000000]: [robocluedo_mission_manager] status: MISSION_STATUS_ASK_ONTOLOGY
	[ INFO] [1660478192.348465100, 356.941000000]: [robocluedo_mission_manager] no brilliant ideas from the RoboCLuedo B.B.B. (Big Brain Bruh) COLLECTING AGAIN
	[ INFO] [1660478192.348579000, 356.941000000]: [robocluedo_mission_manager] status: MISSION_STATUS_COLLECT
	[ WARN] [1660478192.701782900, 357.215000000]: [robocluedo_mission_manager] WARNING: landmark COLLECT no longer applicable (all the waypoints have been explored) TRYING TO directly SOLVE THE MYSTERY
	[ INFO] [1660478192.701833500, 357.215000000]: [robocluedo_mission_manager] status: MISSION_STATUS_ASK_ONTOLOGY
	[ INFO] [1660478192.729748300, 357.237000000]: [robocluedo_mission_manager] no brilliant ideas from the RoboCLuedo B.B.B. (Big Brain Bruh) REPLANNING
	[ INFO] [1660478192.729869700, 357.237000000]: [robocluedo_mission_manager] status: MISSION_STATUS_REPLAN
	[ INFO] [1660478198.633827100, 362.407000000]: [robocluedo_mission_manager] gotta catch 'em all! NA NA NA NA NA NA
	[ INFO] [1660478198.633924300, 362.407000000]: [robocluedo_mission_manager] status: MISSION_STATUS_COLLECT
	[ INFO] [1660478264.932059200, 419.078000000]: [robocluedo_mission_manager] (cbk_oracle_hint, add hint) received hint with content (ID=0, key=where, value=conservatory)
	[ INFO] [1660478265.044341400, 419.157000000]: [robocluedo_mission_manager] (cbk_oracle_hint, add hint) valid hint received and registered
	[ INFO] [1660478280.620287300, 432.618000000]: [robocluedo_mission_manager] status: MISSION_STATUS_ASK_ONTOLOGY
	[ INFO] [1660478280.649785900, 432.651000000]: [robocluedo_mission_manager] no brilliant ideas from the RoboCLuedo B.B.B. (Big Brain Bruh) COLLECTING AGAIN
	[ INFO] [1660478280.649918600, 432.651000000]: [robocluedo_mission_manager] status: MISSION_STATUS_COLLECT
	[ INFO] [1660478329.181607100, 474.499000000]: [robocluedo_mission_manager] (cbk_oracle_hint, add hint) received hint with content (ID=0, key=what, value=candlestick)
	[ INFO] [1660478329.265570900, 474.543000000]: [robocluedo_mission_manager] (cbk_oracle_hint, add hint) valid hint received and registered
	[ INFO] [1660478339.478846100, 483.301000000]: [robocluedo_mission_manager] status: MISSION_STATUS_ASK_ONTOLOGY
	[ INFO] [1660478339.550077100, 483.357000000]: [robocluedo_mission_manager] (from the ontology) found consistent hypotheses : 1
	[ INFO] [1660478339.550119700, 483.357000000]: [robocluedo_mission_manager] status: MISSION_STATUS_SOLVE
	[ INFO] [1660478341.487364700, 484.998000000]: [robocluedo_mission_manager] plan not solvable (need intermediate replanning
	[ INFO] [1660478341.487517600, 484.998000000]: [robocluedo_mission_manager] status: MISSION_STATUS_REPLAN
	[ INFO] [1660478347.442209500, 490.266000000]: [robocluedo_mission_manager] retrying landmark SOLVE after replanning
	[ INFO] [1660478347.442323200, 490.266000000]: [robocluedo_mission_manager] status: MISSION_STATUS_SOLVE
	[ INFO] [1660478446.738491300, 575.338000000]: [robocluedo_mission_manager] status: MISSION_STATUS_ASK_ORACLE
	[ INFO] [1660478446.744114100, 575.344000000]: [robocluedo_mission_manager] Narrator voice: 
		(wispering) RoboCLuedo thinks that the solution is ID=0
	[ INFO] [1660478446.744179800, 575.344000000]: [robocluedo_mission_manager] RoboCLuedo: 
		WELL, missScarlett, DID YOU MURDER Dr. Black ? 

			HUH?


	[ INFO] [1660478446.744212400, 575.344000000]: [robocluedo_mission_manager] Oracle: 
		ID=0 is the way
	[ INFO] [1660478446.744237500, 575.344000000]: [robocluedo_mission_manager] gg ez
	[ INFO] [1660478446.744257000, 575.344000000]: [robocluedo_mission_manager] ==== ID=0 | where[conservatory] what[candlestick] who[missScarlett] ====
	```
	
	*suono di microfono che viene lasciato cadere per terra*.

- **COMMIT** "happy commit!"

---

## 15/08/2022 -- ultime issues -- documentazione del codice e deploy

anzitutto, un piccolo elenco di quello che devo fare per il deployment:

- (OK) -- rimuovere eventuali modalità di backup da in giro per il codice
	- erl2 simulation.cpp (casomai crea una versione alternativa del nodo)
	- mission manager
- revisione del codice
	- (OK) -- migliorare il log dal mission manager
	- ... (magari qualcosa segnato nel TODO?)
	- ultima volta, il module testing (giusto per essere sicuri)
- ultimi diagrammi UML 
	- con un "dizionario" delle interfacce
	- elimina i diagrammi sbagliati o troppo vecchi (da questo progetto esiste uno standard)
	- ... (quali mancano?)
	- ... (sequence diagrams?)
- ultima prova *filmata* del sistema in funziona
	- prima con la modalità di sviluppo nell'oracolo
	- poi senza modalità di sviluppo
- documentazione ausiliaria sui singoli progetti:
	- sarebbe meglio iniziare mettendo a posto le toctree in giro per il codice
	- armor
	- rosplan
	- robot
	- movement controller
	- mission manager
- come runnare il progetto - versione definitiva
	- launch rapido
	- launch nodo per nodo (con descrizione del progetto)
- documentazione del codice
	- creazione delle cartelle source code con il codice dentro
	- prima vedi che esce solo così
	- poi correggi l'output
- ultima revisione della documentazione del codice
- scrittura del README
	- struttura (classica) del readme
	- e scrittura del contenuto (molto può essere rimandato alla documentazione)
- e caricamento della documentazione su GitHub

devo fare almeno parte di questo lavoro se voglio iniziare il terzo assignment, che consisterà in una leggera modifica di questo secondo assignment e nulla di più. 

---

andiamo col lavoro:

- via tutte le modalità di sviluppo
	- da simulation
	- la threshold a 0.35 ieri funzionava bene ... se la abbassassi a 0.3? 
		- proviamo
		- *posso abbassarlo ancora*, stavolta a 0.25
		- anche a 0.25 (la distanza *iniziale*) pare funzionare, e non dovrebbe missarne nessuno
	- *direi di lasciare la distanza a 0.25 com'era all'inizio*
- già che ci sono, rivedo i log dal mission manager
	- *suspence*
	- ora il nodo ha anche DEVELOP_PRINT_ENABLED che permette di stampare quei print che di norma non sarebbe utile stampare. 
	- proviamo se funziona, giusto per accertarmi di non aver scritto castronerie
	- funziona tutto a dovere. Ottimo.
	- **COMMIT**: "deployment (logs more clear from the mission manager)"

---

ora, i diagrammi UML mancanti. direi di andare per package:

- giusto per non doverli riscrivere tutte le volte, ecco qualche template per i servizi e i topic in UML: (avrei dovuto fare prima questo lavoro...)

**topics**:

```
``\`
@startuml
	
() "/???" as TOPIC_???
''' TOPIC_??? <-- "pub" NODE_???
''' TOPIC_??? --> "sub" NODE_???
note on link: ???/???.msg
	
@enduml
``\`
```

**services:**

```
``\`
@startuml

() "/???" as SRV_???
''' SRV_??? <-- "srv" NODE_???
''' SRV_??? --> "cl" NODE_???
note on link: ???/???.srv

@enduml
``\`
```

- iniziamo dal package di aRMOR e armor msgs
	- documentazione delle interfacce
	- ora c'è anche un giga UML per aRMOR, direi che siamo soddisfatti
	- mancano gli esempi, che mi tocca riconvertire da Doxygen a Sphinx
		- peccato però, lo stile di Doxygen mi piaceva abbastanza ... graficamente orrendo, ma metteva i link belli sistemati in automatico
	- e serve anche un documento in cui trascrivere i comandi aRMOR dal module testing
	- ora manca solo la documentazione del codice, che peraltro è già fatta: devo solo espanderla
	- se non ricordo male, avevo scritto anche delle informazioni su come configurare il componente... al vecchio progetto!
- mi sono accorto che nel package erano rimasti dei vecchi parametri ... togliendoli non esploder aRMOR, vero? 
	- ok ha funzionato tutto, quindi quei parametri erano inutili qui
	- posso anche eliminare i file contenenti gli items (la cartella cluedo items)
- e con aRMOR abbiamo chiuso!
- **COMMIT**: "deployment (armor documentation)"

---

meglio fare quello che so di non dover andare a ritoccare nel terzo assignment: devo fare solo le cose su cui so che non ci metterò mai più mano

- robot urdf documentazione
	- ...e invece direi che va bene, anche se il robot mostrato è ancora quello con le ruote più lontane dal centro
- nello user manual di erl2 non si fa riferimento alle funzionalità di sviluppo
	- le aggiungo ora
- per quanto riguarda i file worlds, aspetterei il terzo progetto per fare qualunque modifica:ç ho tutti quei file world da provare ... lo farò poi
- **COMMIT** -- "deploy (dependencies documentation)"

---

il mio obiettivo è solo di *dare una ripulita* prima di iniziare ad implementare il prossimo assignment

- direi che la documentazione riferita ai pacchetti di messaggi ... non serve, a meno di non trovare un modo per forzare Sphinx ad importarli nella documentazione, il che sarebbe veramente molto bello. 
	- *la scomoda domanda: esiste un modo per importare i file .yaml come si fa per il codice?*
		- questo è interessante: [sphinx](http://wiki.ros.org/Sphinx)
		- e anche questo: [rosdoc](http://wiki.ros.org/rosdoc_lite)
		- anche questo sembra piuttosto interessante [qui](https://www.astro.rug.nl/~vogelaar/sphinx/build/html/index.html)
	- **ma nulla di convincente finora**
	- questo mi sembra la soluzione migliore: [sphinxcontrob-yaml](https://pypi.org/project/sphinxcontrib-yaml/)
		- proviamolo, sembra interessante
		- giusto un minimo di documentazione preliminare, che non guasta mai
		- *momento della verità...*
		- ... e logicamente non funziona
- per ora *mi arrendo* , ma solo *per ora*
- **COMMIT** : "deployment (testing autoyaml ... not working)"

--- 

dopo questo excursus che mi ha solo fatto venire il nervoso, andiamo avanti

- ultime cose coi package per i messaggi (sarebbe bello avere un modo per importare i file yaml dentro alla documentazione, così da poter documentare anche i messaggi senza dover fare dell'orrendo copia incolla)
- ultima sistemata ai packages coi messaggi
- documentazione del codice per il module testing (veramente poca roba)
	- ...avevo dimeticato che per i moduli python servisse il mocking...
	- ... in più devi aggiungere i percorsi a mano in sphinx
- a questo punto per ora facciamo solo la generazione delle code reference
	- movement controller
	- rosplan
- **COMMIT**: "deployment (setup code docs)"

--- 

- mi sono dimenticato il mission manager!
- **COMMIT**: "deployment (pkg mission manager docs setup)"
















## TODO

- ma provando con un controller per uno skid robot cosa succede? magari riesco a farlo andare meglio
- vanno aggiustati i guadagni dei go to point e align, e magari introdurre anche una saturation
- navigation system: creare un controller che gestisca anche il braccio durante la navigazione (magari abbassarlo per rendere il robot più stabile)
- documentazione del package ROSPlan
- UML finale per ROSPlan
- *module testing* su kb_tools
- evita di muovere il braccio robotico se sai che la posture richiesta è uguale a quella attuale, *memorizza la posture attuale* (puoi tracciarla...)
- pensare bene al module testing, e in particolare correggere i module testing di armor (potrebbero non funzionare più per via delle ultime modifiche ai servizi)
- *un tantino in più di scena nell'indagine?* (intanto deve funzionare...)

- implementazione precisa dell'azione move-to
- implementazione del launch di un particolare file di configurazione per rviz

- la pagina di documentazione del codice andrebbe messa in una cartella chiamata code_documentation (inutile dire che ogni package ha le sue, e non si deve mischiare nulla)
- rifare gli schemi di armor! 
- aggiornare schema feedback manager
- aggiustare le toctree in giro per le pagine
- creare una "rubrica" generale di topic e servizi dell'architettura
- e creare uno standard per gli UML in ROS, o qualcosa che possa essere replicato
- **vanno fatti ancora i temporal diagrams!** e i diagrammi riguardanti l'architettura generale
- in tutti gli UML dei componenti delle implementazioni delle azioni PDDL c'è una parte comune di feedback: scrivila una volta per tutte e copia-incolla in tutti i diagrammi
- aggiungere le pagine di documentazione del codice di Sphinx!
- aggiornare i template delle actions in questo file
- rimuovere le immagini dalla documentazione del codice per armor
- un branch per la documentazione Sphinx
- e uno script per mettere online la documentazione sphinx "senza sbattersi troppo"
- approfondire UML armor
- aggiornare il file di descrizione e design del PDDL con il codice corretto
	- correggere i landmark, oppure eliminare direttamente la sezione
- specificare negli UML, nel tipo di topic/servizio, *anche il formato del tipo*, ad esempio se è *.msg* o anche *.srv*

- autenticazione SSH sul Docker di lavoro


## NOTE

### una nota su rosplan e il mission manager

il principio è questo:

- *il mission manager* si occupa del progresso della missione ad alto livello
- *ROSPlan* si preoccupa della singola azione da eseguire, *nel dettaglio dello stato delle cose*. 

Quindi il comportamento del robot è distribuito su due livelli: il primo è quello "generico" del mission manager, che si occupa anche di fare da mediatore *tra due componenti architetturali diversi* (armor e rosplan), e il secondo quello specifico che si preoccupa di gestire i dettagli delle varie azioni da compiere. 

in questo caso la sequenza delle azioni non è molto complessa, però in futuro potrebbe esserla. ad esempio, il landmark COLLECT, che attualmente comprende una sequenza di 3 azioni (più la scelta di dove andare in base alla mappa topologica dell'area ... ok, non così semplice) potrebbe, in futuro, comprenderne di più: ad esempio, comprendere movimenti espliciti del manipolatore in base al tipo di hint da raccogliere, determinate operazioni di sensing durante la raccolta, e molto di più. 

più il comportamento del robot evolve in complessità, più è necessaria una separazione tra diversi modi di vedere il da farsi. più di due livelli però ... non so quanto convenga. 2 è la scelta più logica ed efficace per la maggioranza dei casi. 

### vantaggi (e svantaggi) dell'approccio per landmarks

motivazioni per cui ho scelto un planning su due livelli e l'approccio dei landmarks:

- maggiore flessibilità in caso di modifica dell'architettura
- il planner PDDL non deve fare un'intera pianificazione statica, perciò è più veloce nel risolvere il plan
- fare un planning statico dell'intera situazione è semplicemente assurdo, se pensiamo a tutti i problemi imprevisti che possono sorgere
- vedere il comportamento del sistema su due livelli permette di astrarre in maniera più conveniente

problemi, limitazioni, difetti:

- rapido aumento della complessità di implementazione del sistema


