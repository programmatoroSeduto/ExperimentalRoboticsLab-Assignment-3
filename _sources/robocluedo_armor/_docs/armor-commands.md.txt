
# aRMOR commands 

---

```{toctree}
---
cation: contents:
maxdepth: 2
---
./armor-commands.md
```

---

## Main directives

here's a list of the most important aRMOR directives employed in this project. 

```c++
/*
 * disjoint di classe da un'altra
 * 	"DISJOINT", "CLASS", "", "PERSON", "WEAPON"
 * 	--
 * 
 * disjoint individual qualunque da individual qualunque
 * 	"DISJOINT", "IND", "", "Lounge", "HP3"
 * 	--
 * 
 * aggiungi un individual ad una classe
 * 	"ADD", "IND", "CLASS", "Jim", "PERSON" 
 * 	--
 * 
 * ritorna tutti gli individual appartenenti ad una classe
 * 	"QUERY", "IND", "CLASS", "HYPOTHESIS"
 * 	<uri#value>
 * 
 * aggiungi una proprietà
 * 	"ADD", "OBJECTPROP", "IND", "who", "HP3", "Jim"
 * 	--
 * 
 * query sulle proprietà di uno specifico individual
 * (logica: "QUERY" - "su cosa voglio lavorare" - "cosa voglio ottenere")
 * 	"QUERY", "IND", "OBJECTPROP", "HP3" 
 * 	<uri#property>
 * 
 * update
 * 	"REASON"
 * 	--
 * 
 * save
 * 	"SAVE", "INFERENCE", "", "/root/Desktop/ROBOCLUEDO_ONTOLOGY.owl"
 * 	--
 * 
 * check ipotesi consistenti
 * 	"QUERY", "IND", "CLASS", "COMPLETED"
 * 	<uri#HP3>
 * 
 * classe/i di un individual
 * 	"QUERY", "CLASS", "IND", "HP3", "false"
 * 	<uri#HYPOTHESIS>
 * 
 * classe più profonda di un individual (BUGGATO: non funziona, vedi module testing)
 * 	"QUERY", "CLASS", "IND", "HP3", "true"
 * 	HYPOTHESIS
 * 
 * Ritorna i valori associati ad una certa proprietà 
 * 	"QUERY", "OBJECTPROP", "IND", "where", "HP3"
 * 	<http://www.emarolab.it/cluedo-ontology#Study>
 * 	corrisponde a: (HP3, Study):where
 * 
 */	
```

## "QUERY CLASS IND" buggy

see the output from the node `robocluedo_module_testing test_armor`. 
