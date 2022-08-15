(define (problem task)
(:domain dom)
(:objects
    b1 b2 b3 - boolobj
    kk1 kk2 kk3 kk4 - key
)
(:init
    (b-not-true b2)

    (b-true b1)
    (b-true b3)


    (stop)


    (= (f-non-zero) 5)

    (= (f-zero) 0)


)
(:goal (and
    (b-true b1)
    (b-true b2)
    (b-not-true b3)
    (stop)
))
)
