
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


