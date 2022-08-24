# robocluedo -- notes about the PDDL 

## osservazioni iniziali

- il planner risolve il problema fino alla fine come se non ci fosse nessun problema
- è il sistema esterno a risolvere gli imprevisti con eventuali richieste di replanning
- la knowledge base è formata da un insieme di informazioni non comunicanti: quelle che servono per il planner PDDL, e quelle che servono per la reale gestione del reasoning
- date le funzionalità della knowledge base, contare via query è davvero una pessima idea: il PDDL dovrà portare con se tutta una serie di metadati da aggiornare ogni volta che viene ricevuto un nuovo hint dall'oracolo. questa operazione permette inoltre di usare la knowledge base come una shared memory tra le varie actions
- ogni nodo ha la sua particolare istanza di `robocluedo_kb_tools`, per giunta su processi diversi, dunque l'unica vera memoria condivisa è la knowledge base: inutile tenere dei conteggi nella classe senza aggiornare la knowledge base: dei fluents di conteggio devono esserci sempre nel problema. 

## system inizialization

the system shall be initialized, which means to allocate the hints and to update the ontology before starting. 

```
(init-planning-system )
	(*) controlla e aggiorna lo stato del database
		(*) conta gli elementi associati alle ipotesi
		(*) e classifica le ipotesi nelle tre categorie
	(*) controlla che il problema ammetta soluzione
		# c'è almeno un'ipotesi ammissibile?
		(*) IF almeno una ipotesi ammissibile -> REPLAN (verso la soluzione)
	(*) applica l'effetto dell'azione
```

### check hint status

**assunzione** : dopo un tentativo di esecuzione di soluzione fallito, la knowledge base conserva ancora i dati derivati dall'ultima esecuzione. 

classificazione degli ID:

- un ID è *attivo* quando non si può provare che questi non è valido
- un ID è *escluso*  quando uno tra i campi *who*, *where* o *what* ha almeno due elementi associati
	questo è gestito dall'esterno: è il sistema esterno al PDDL che individua le esclusioni. il planner può escludere solo questi casi
- un ID è *ammissibile* quando i tre campi hanno uno e un solo elemento

il sistema iniziale valuta le condizioni delle ipotesi e marca i tre predicati col loro stato. 

il sistema fa anche il check se il problema è risolvibile. 

**il problema è risolvibile quando** esiste almeno un'opzione ancora attiva

**il problema è risolto per esclusione** quando esiste un'unica opzione valida o anche completa

**il problema non è risolvibile** quando tutte le opzioni sono state escluse

## movimento

caratteristiche generali:

- il robot può muoversi ovunque voglia, anche al centro
- i punti in cui il robot può stare sono chiamati waypoints nel codice 

constraints:

- il robot non può stare più di un turno nello stesso posto 
- (però può tornare al punto precedente al turno successivo, basta che si muova)

pseudocodice associato all'azione:

```
(move-to ?from ?to )
	# serve un check sulla validità delle preconditions prima di iniziare?
	(*) ottieni le coordinate reali del waypoint
		# esiste una tabella nella classe che associa il waypoint alla pose reale
		# oppure leggi le posizioni dall'oracolo
	(*) navigation controller -> vai a quel waypoint
		# attendi fin quando il robot non ha raggiunto la posizione finale
	(*) applica l'effetto dell'azione
```

## segnale di acquisizione

il robot può continuare a muoversi nello spazio come vuole. Al momento però in cui bisogna fare l'acquisizione di un hint, il robot deve *segnalare* la sua volontà di acquisire il suggerimento. 

```
(signal-info-acquisition ?wp)
	# controlla le preconditions
	(*) aggiorna allo stato successivo
```

anche una semplice azione standard è sufficiente per questa operazione. 

## avvicinamento manipolatore

**quando il robot avvicina il manipolatore, l'oracolo pubblica un messaggio con l'hint**

```
(manipulator-near-marker ?wp )
	# check sulla validità delle condition?
	(*) coordinate del marker
	(*) manipulator controller -> avvicina il manipolatore
		# attendi fin quando l'azione non ha avuto termine
	# a questo punto, l'azione successiva dovrebbe aver ricevuto l'hint
	(*) fine dell'azione, aggiorna la knowledge base
```

## cattura dell'indizio

qui il robot "legge" l'indizio, vale a dire il sistema interpreta l'indizio proveniente dall'oracolo (precedentemente allocato)

```
(acquire-hint ?wp)
	# controllo delle preconditions?
	(*) controlla che l'indizio ricevuto sia ben formato
		#    (altrimenti scartalo e passa avanti)
		# controllare qui se l'indizio è ridondante?
		(*) aggiungi l'indizio alla ontology
		(*) in base all'indizio, classifica l'ID dell'ipotesi implicata
	(*) INFINE applica il nuovo stato alla KB
```

## riprotare il manipolatore alla posizione iniziale

a questo punto il robot ha già acquisito l'indizio. 

```
(manipulator-far-marker ?wp )
	# check della validità delle preconditions? 
	(*) manipulator controller -> home position del manipolatore
		# attendi fino al termine dell'operazione
	(*) aggiorna la KB
```

## valutazione dello stato del problema

questa azione viene eseguita subito dopo aver ritratto il manipolatore dopo l'acquisizione dell'hint, e consiste nella valutazione di risolubilità del problema in base allo stato attuale e al numero di mosse rimanenti. 

Non conviene fare questa mossa quando il manipolatore è ancora vicino al marker. 

```
(sherlok-is-thinking ?wp )
	# check delle conditions? 
	(*) aggiorna i conteggi
	(*) ->VERIFICA SE IL PROBLEMA E' ANCORA RISOLVIBILE
		# questa procedura non dovrebbe essere eseguita una volta ritratto il braccio?
		(*) ELIF nessun ID valido : FINE, mistero irrisolvibile
		(*) ELIF c'è solo un ID valido : REPLAN, verso una soluzione per esclusione
		(*) ELIF più di un ID : 
			(*) IF troppi ID, poche mosse -> REPLAN
			(*) ELSE : vai avanti
	(*) nel caso tutto sia andato bene, aggiorna la KB e vai avanti
```

## preparazione alla soluzione finale

l'idea di base: il robot può proporre la sua soluzione solo una volta raggiunto il centro dell'arena. 

```
(bright-bulb-over-head ?wp )
	# check delle conditions? 
	(*) aggiorna la KB e vai avanti
```

## la soluzione del caso

il planner, dato che funziona offline, non può ricevere alcun dato sui vari ID delle ipotesi: purchè un'ipotesi sia classificata valida, il planner può sempre sceglierla. Sempre nello stesso spirito: *per il planner, tutto va a liscio liscio*. E' il sistema esterno che deve giudicare la soluzione proposta, evitando decisioni "eccessivamente irrazionali" da parte del planner, che comunque attua una scelta al termine dell'investigazione, anche se non dettata da un effettivo ragionamento deduttivo. 

il planner può scegliere tra le opzioni attive, ed escluderà solo quelle risapute non valide *a priori*, quindi quelle escluse dal problem file prima di fare la ricerca della soluzione. 

per favorire le opzioni complete su quelle solo attive, il codice PDDL richiede un'ottimizzazione sulla qualità della soluzione, e mette a disposizione due tipi di azione: q1 che aggiunge 1 alla qualità, e q2 che invece aggiunge 10 alla qualità. entrambe hanno lo stesso effetto, ma il reward di una è maggiore che nell'altra. 

```
(who-killed-doctor-black-huh-q1 ?id )
(who-killed-doctor-black-huh-q2 ?id )
	# controlla le preconditions
	(*) IF id di opzione classificata completa : 
		(*) oracolo -> invia la soluzione e ricevi l'id della soluzione
			# servizio, bloccante
		(*) IF soluzione corretta : SOLUZIONE DEL CASO, termina la missione
		(*) ELSE : segna l'esito nella KB
			# e passa alla valutazione di fattibilità
	(*) ELIF id classificato attivo : REPLAN, non accettare una soluzione non motivata
	(*) valutazione di fattibilità
		(*) il problema è risolvibile? ci sono ancora ipotesi smarcate?
			# se non c'è più nulla da fare, FAIL 
		(*) valuta il numero di cicli alla prossima iterazione
			# se c'è un solo ID completo da provare, 0 potrebbe essere la scelta
			# SLOT : (3 - massimo numero di elementi in una ipotesi ancora aperta) * (numero di ID ancora attivi)
			# in generale meno slot ci sono da riempire, e meno iterazioni serviranno per proporre una ipotesi completa
	(*) infine, REPLAN e il ciclo ricomincia
```

### movimento verso il centro

il robot si muove verso il centro dell'arena per proporre la soluzione. 

## Landmarks

riporto qui i landmarks che ho usato per debuggare e testare il codice PDDL, tutti a partire dallo stesso stato iniziale. 

### solo init del sistema

il sistema parte da stato `(pre-init )`. Nota che, di tutti questi stati, ce n'è ad ogni azione solo uno acceso, come una sorta di segnalatore di stato. 

```lisp
(:goal (investigating ))
```

### init e movimento a w2

il robot parte dal centro e arriva in w2

```lisp
(:goal (and (investigating ) (robot-position w2)))
```

o anche solo 

```lisp
(:goal (robot-position w2))
```

### in w2 e braccio verso il marker

```lisp
(:goal (and (robot-position w2 ) (acquiring ) (manipulator-on )))
```

o anche, più compatto,

```lisp
(:goal (and (robot-position w2 ) (acquiring ) (manipulator-on )))
```

### cattura il marker w2 col braccio vicino al marker

```lisp
(:goal (and (robot-position w2 ) (acquired ) (manipulator-on ) (acquired-hint w2)))
```

in alternativa, bastava anche solo

```lisp
(:goal (acquired-hint w2))
```

### wp2 acquisito e manipolatore non attivo

```lisp
(:goal (and (manipulator-off ) (acquired-hint w2)))
```

### wp2 e wp3 in due mosse

il fluent `(remaining-moves )` viene inizializzato a 3 prima di far partire il problema. 

```lisp
(:goal (and (manipulator-off ) (acquired-hint w2 ) (acquired-hint w3 ) (>= (remaining-moves ) 1)))
```

si ottiene un risultato praticamente identico usando la metrica:

```lisp
(:goal (and (manipulator-off ) (acquired-hint w2 ) (acquired-hint w3 )))

(:metric maximize (remaining-moves ))
```

### wp1 wp2 wp3 poi in mezzo

```lisp
(:goal (and 
	(acquired-hint w1 )
	(acquired-hint w2 )
	(acquired-hint w3 )
	(robot-position center )
))
```

### ciclo completo di indagine prima della soluzione

```lisp
(:goal (wanna-solve ))
```

### in centro per proporre la soluzione

```lisp
(:goal (and
	(wanna-solve ) (robot-position center)
))
```

### il ciclo di lavoro completo

```lisp
(:goal (elementary-whatson ))

(:metric maximize (solution-quality ))
```

## Checkpoints

### domain

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
	;; ontology
	hypID
	
	;; navigation system
	waypoint
)

(:constants
	center - waypoint
)

(:predicates
	;; working status
	(pre-init )
	(investigating )
	(ready-to-acquire )
	(acquiring )
	(acquired )
	(thinking )
	(wanna-solve )
	(who-killed-doctor-black )
	(elementary-whatson )
	
	;; environment
	(passage ?w1 ?w2 - waypoint )
	
	;; navigation system
	(not-explored ?w - waypoint )
	(robot-position ?w - waypoint )
	
	;; manipulator
	(manipulator-off )
	(manipulator-on )
	
	;; hint gathering
	(not-acquired-hint ?wp - waypoint )
	(acquired-hint ?wp - waypoint )
	(can-give-hint ?wp - waypoint )
	
	;; hypotheses classification
	(h-open ?id - hypID )
	(h-complete ?id - hypID )
	(h-discard ?id - hypID )
)

(:functions
	(remaining-moves )
	(solution-quality )
)

;; === SYSTEM INIT === ;;

(:durative-action init-planning-system
	:parameters ( )
	
	:duration (= ?duration 1)
	
	:condition (at start (pre-init ))
	
	:effect (and
		(at start (not (pre-init )))
		(at end (investigating ))
	)
)

;; === NAVIGATION SYSTEM === ;;

(:durative-action move-to
	:parameters ( ?from ?to - waypoint )
	
	:duration (= ?duration 1)
	
	:condition (and
		;; status
		(at start (investigating ))
		
		;; localization
		(at start (robot-position ?from ))
		
		;; passage
		(at start (passage ?from ?to ))
		
		;; path tracking
		(at start (not-explored ?to ))
		
		;; manipulator
		(at start (manipulator-off ))
		
		;; it must be a new hint there
		; (at start (can-give-hint ?to ))
		; (at start (not-acquired-hint ?to ))
	)
	
	:effect (and
		;; localization
		(at start (not (robot-position ?from )))
		(at end (robot-position ?to ))
		
		;; path tracking
		(at end (not (not-explored ?to )))
		(at end (not-explored ?from ))
	)
)

(:durative-action move-to-center
	:parameters ( ?from - waypoint )
	
	:duration (= ?duration 1)
	
	:condition (and
		;; status
		(at start (wanna-solve ))
		
		;; localization
		(at start (robot-position ?from ))
		
		;; moves
		(at start (= (remaining-moves ) 0))
	)
	
	:effect (and
		;; localization
		(at start (not (robot-position ?from )))
		(at end (robot-position center))
		
		;; system status
		(at start (not (wanna-solve )))
		(at end (who-killed-doctor-black ))
	)
)

;; === MANIPULATOR === ;;

(:durative-action manipulator-near-marker
	:parameters ( ?wp - waypoint )
	
	:duration (= ?duration 1)
	
	:condition (and
		;; robot status
		(at start (ready-to-acquire ))
		
		;; position
		(at start (robot-position ?wp ))
		
		;; it must be a new hint there
		(at start (can-give-hint ?wp ))
		(at start (not-acquired-hint ?wp ))
		
		;; manipulator
		(at start (manipulator-off ))
	)
	
	:effect (and
		;; robot status
		(at start (not (ready-to-acquire )))
		(at end (acquiring ))
		
		;; manipulator status
		(at start (not (manipulator-off )))
		(at end (manipulator-on ))
	)
)

(:durative-action manipulator-far-marker
	:parameters ( ?wp - waypoint )
	
	:duration (= ?duration 1)
	
	:condition (and
		;; robot status
		(at start (acquired ))
		
		;; position
		(at start (robot-position ?wp ))
		
		;; point already acquired
		(at start (can-give-hint ?wp ))
		(at start (acquired-hint ?wp ))
		
		;; manipulator status
		(at start (manipulator-on ))
	)
	
	:effect (and
		;; robot status
		(at start (not (acquired )))
		(at end (thinking ))
		
		;; manipulator status
		(at start (not (manipulator-on )))
		(at end (manipulator-off ))
	)
)

;; === HINT ACQUISITION === ;;

(:durative-action signal-info-acquisition
	:parameters ( ?wp - waypoint )
	
	:duration (= ?duration 1)
	
	:condition (and
		;; robot status
		(at start (investigating ))
		
		;; manipulator status
		(at start (manipulator-off ))
		
		;; robot position
		(at start (robot-position ?wp ))
		
		;; moves
		(at start (> (remaining-moves ) 0))
	)
	
	:effect (and
		;; moves
		(at end (decrease (remaining-moves ) 1))
		
		;; robot status
		(at start (not (investigating )))
		(at end (ready-to-acquire ))
	)
)

(:durative-action acquire-hint
	:parameters ( ?wp - waypoint )
	
	:duration (= ?duration 1)
	
	:condition (and
		;; position
		(at start (robot-position ?wp ))
		
		;; robot state
		(at start (acquiring ))
		
		;; manipulator state
		(at start (manipulator-on ))
		
		;; it must be a hint there
		(at start (can-give-hint ?wp ))
		(at start (not-acquired-hint ?wp ))
	)
	
	:effect (and
		;; robot state
		(at end (not (acquiring )))
		(at end (acquired ))
		
		;; marker has been acquired
		(at end (not (not-acquired-hint ?wp )))
		(at end (acquired-hint ?wp ))
	)
)

(:durative-action sherlock-is-thinking
	:parameters ( ?wp - waypoint )
	
	:duration (= ?duration 1)
	
	:condition (and
		;; system status
		(at start (thinking ))
		
		;; manipulator status
		(at start (manipulator-off ))
		
		;; robot localization
		(at start (robot-position ?wp ))
		
		;; hint already acquired for that position
		(at start (can-give-hint ?wp ))
		(at start (acquired-hint ?wp ))
	)
	
	:effect (and
		;; system status
		(at start (not (thinking )))
		(at end (investigating ))
	)
)

;; === MYSTERY SOLUTION === ;;

(:durative-action bright-bulb-over-head
	:parameters ( ?wp - waypoint )
	
	:duration (= ?duration 1)
	
	:condition (and
		;; system status
		(at start (investigating ))
		
		;; robot position
		(at start (robot-position ?wp ))
		
		;; no more moves
		(at start (= (remaining-moves ) 0))
	)
	
	:effect (and
		(at start (not (investigating )))
		(at end (wanna-solve ))
	)
)

(:durative-action who-killed-doctor-black-huh-q1
	:parameters ( ?id - hypID )
	
	:duration (= ?duration 1)
	
	:condition (and
		;; system status
		(at start (who-killed-doctor-black ))
		
		;; robot position
		(at start (robot-position center))
		
		;; remaining moves
		(at start (= (remaining-moves ) 0))
		
		;; quality of the ID -- active
		(at start (h-open ?id ))
	)
	
	:effect (and
		;; reward - 1 point
		(at end (increase (solution-quality ) 1))
		
		;; status change
		(at start (not (who-killed-doctor-black )))
		(at end (elementary-whatson ))
	)
)

(:durative-action who-killed-doctor-black-huh-q2
	:parameters ( ?id - hypID )
	
	:duration (= ?duration 1)
	
	:condition (and
		;; system status
		(at start (who-killed-doctor-black ))
		
		;; robot position
		(at start (robot-position center))
		
		;; remaining moves
		(at start (= (remaining-moves ) 0))
		
		;; quality of the ID -- complete
		(at start (h-complete ?id ))
	)
	
	:effect (and
		;; reward - 10 points
		(at end (increase (solution-quality ) 10))
		
		;; status change
		(at start (not (who-killed-doctor-black )))
		(at end (elementary-whatson ))
	)
)

)

```

### problem

```lisp
(define (problem task)

(:domain robocluedo)

(:objects
	;; ontology 
	ID1 ID2 ID3 ID4 ID5 ID6 - hypID
	
	;; environment
	w1 w2 w3 w4 - waypoint
)

(:init
	;; init state of the system
	(pre-init )
	
	;; moves
	(= (remaining-moves ) 3)
	
	;; quality of the solution
	(= (solution-quality ) 0)
	
	;; environment data
	(passage w1 w2 )
		(passage w1 w3 )
		(passage w1 w4 )
		(passage w1 center )
	(passage w2 w1 )
		(passage w2 w3 )
		(passage w2 w4 )
		(passage w2 center )
	(passage w3 w1 )
		(passage w3 w2 )
		(passage w3 w4 )
		(passage w3 center )
	(passage w4 w1 )
		(passage w4 w2 )
		(passage w4 w3 )
		(passage w4 center )
	(passage center w1 )
		(passage center w2 )
		(passage center w3 )
		(passage center w4 )
	
	;; navigation system
	(robot-position center )
	(not-explored w1 )
	(not-explored w2 )
	(not-explored w3 )
	(not-explored w4 )
	
	;; manipulator
	(manipulator-off )
	
	;; hints gathering system
	(can-give-hint w1 )
		(not-acquired-hint w1 )
	(can-give-hint w2 )
		(not-acquired-hint w2 )
	(can-give-hint w3 )
		(not-acquired-hint w3 )
	(can-give-hint w4 )
		(not-acquired-hint w1 )
	
	;; hypotheses classification
	(h-open ID1 )
	(h-open ID2 )
	(h-open ID3 )
	(h-open ID4 )
	(h-open ID5 )
	(h-open ID6 )
)

(:goal (elementary-whatson ))

(:metric maximize (solution-quality ))

)

```

### ultimo log

```
Number of literals: 27
Constructing lookup tables: [10%] [20%] [30%] [40%] [50%] [60%] [70%] [80%] [90%] [100%]
Post filtering unreachable actions:  [10%] [20%] [30%] [40%] [50%] [60%] [70%] [80%] [90%] [100%]
No analytic limits found, not considering limit effects of goal-only operators
47% of the ground temporal actions in this problem are compression-safe
Initial heuristic = 4.000
b (3.000 | 1.000)
Resorting to best-first search
b (3.000 | 1.000)b (2.000 | 20.019)b (1.000 | 21.020);;;; Solution Found
; States evaluated: 1109
; Cost: 0.000
; Time 0.63
0.000: (init-planning-system)  [1.000]
1.001: (move-to center w1)  [1.000]
2.002: (signal-info-acquisition w1)  [1.000]
3.003: (manipulator-near-marker w1)  [1.000]
4.004: (acquire-hint w1)  [1.000]
5.005: (manipulator-far-marker w1)  [1.000]
6.006: (sherlock-is-thinking w1)  [1.000]
7.007: (move-to w1 w2)  [1.000]
8.008: (signal-info-acquisition w2)  [1.000]
9.009: (manipulator-near-marker w2)  [1.000]
10.010: (acquire-hint w2)  [1.000]
11.011: (manipulator-far-marker w2)  [1.000]
12.012: (sherlock-is-thinking w2)  [1.000]
13.013: (move-to w2 w3)  [1.000]
14.014: (signal-info-acquisition w3)  [1.000]
15.015: (manipulator-near-marker w3)  [1.000]
16.016: (acquire-hint w3)  [1.000]
17.017: (manipulator-far-marker w3)  [1.000]
18.018: (sherlock-is-thinking w3)  [1.000]
19.019: (bright-bulb-over-head w3)  [1.000]
20.020: (move-to-center w3)  [1.000]
21.021: (who-killed-doctor-black-huh-q1 id1)  [1.000]
```



















```

```
