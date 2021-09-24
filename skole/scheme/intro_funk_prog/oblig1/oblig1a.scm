(require r5rs)

;;---------------------------------------------------------------
;;
;; Oblig 1. Inf 2810. Christopher Storheim
;;
;;---------------------------------------------------------------

;;--------- OPPGAVE 1 ----------


;;-- a
(* (+ 2 2) 5)
;; Evaluert til: 20
;; Aritmetiske (i dette tilfellet) uttryk begrenset av parenteser.
;; Operator er prefix og operandene etterfoelger som
;; elementer i en liste av argumenter til operator.
;; Hvert uttryk begrenset av parentes vil vaere en
;; sub-kombinasjon til det endelige resultatet (20).

;;-- b
(* (+ 2 2) (5))
;; Resultat:
;; application: not a procedure;
;; expected a procedure that can be applied to arguments
;;  given: 5
;;  arguments...: [none]

;; Begrunnelse:
;; Tallet 5 omsluttet av parentes er ingen "combination" ei
;; heller en prosedyre/operand og foelgelig ikke evaluerbar.

;;-- c
(* (2 + 2) 5)
;; Resultat:
;; application: not a procedure
;; expected a procedure that can be applied to arguments
;;  given: 2
;;  arguments...:
;;   #<procedure:+>
;;   2

;; Begrunnelse:
;; Feilen skyldes in-fix notasjon hvis meningen er aa summere.

;;-- d
(define bar (/ 4 2))
;; Evaluert til: 2
;; Her defineres kombinasjonen (/ 4 2) til aa referere til objektet
;; ved navn bar.

;;-- e
(- bar 2)
;; Evalueres til: 0
;; bar er definert til aa vaere kombinsjonen (/ 4 2)
;; og referer til tallet 2 i kombinsjonen.
;; 2 (bar) trekkes i fra 2 og differansen blir 0

;;-- f
(/ (* bar 3 4 1) bar)
;; Evalueres til: 12
;; Indre kombinasjon er en multiplikasjon av operandene, hvor
;; resultatet evalueres til dividend i ytre kombinasjon.
;; bar refererer fortsatt til kvotienten 2 og blir her
;; divisor i kombinasjonen.


;;--------- OPPGAVE 2 ----------


;;-- 2A 

(or (= 1 2)
    "piff!"
    "paff!"
    (zero? (- 1 1)))
;; Resultat: "piff!"
;; Her evalueres alle elementene/uttrykkene elementvis fra venstre
;; mot hoeyre i Or-kombinasjonen. Det foerste elementet som evalueres
;; til 'true' returneres "umiddelbart", d.v.s uten at de etterfoelgene
;; evalueres. I dette tilfellet, siden en streng ikke er av typen boolean,
;; vil den evaluere strengen til #t. Dette gjelder for bolske prosedyrer
;; som (or ...), (and ...) og (not ...).
;; eks.:
;; (boolean? "piff!" => #f (ikke en bolsk verdi)
;; (not "piff!") => #f (ergo evaluert til #t i en bolsk-kontekst)

(and (= 1 2)
    "piff!"
    "paff!"
    (zero? (- 1 1)))
;; Resultat: #f
;; For (and ...) vil verdien av det foerste uttrykket som evalueres til #f
;; returneres uten videre evaluering av prosedyren. 1 er ikke lik 2
;; og verdien #f returneres.

(if (positive? 42)
    "poff!"
    (i-am-undefined))
;; Resultat "poff!"
;; 42 er positivt og konsekvensen er at prosedyren returnerer "poff!"
;; alternativet (i-am-undefined) blir da ikke evaluert og av den
;; grunn ingen feilmelding.



;;-- 2B 

;; Prosedyre bygd paa if:

(define sign1
  (lambda (x)
    (if (zero? x) 
	0
	(if (> x 0)
	    1
	    -1))))

;; Hvis tallet er null, returner null. Hvis ikke; hvis tallet er
;; stoerre enn null returner 1. Ellers -1
(sign1 -12) 

;; Prosedyre bygd paa cond:

(define sign2
  (lambda (y)
    (cond ((< y 0) -1)
	((> y 0) 1)
	(else 0))))

;; test y stoerre enn null; returner: -1
;; test om y stoerre enn null; returner: 1
;; ellers returner 0

(sign2 0)


;;-- 2C

;; Prosedyre basert paa and/or 

(define sign3
  (lambda (z)
    (or (and (< z 0) -1) (and (> z 0) 1) (and (equal? z 0) 0))))
;; -'z' evalueres i and-uttrykkene.
;; -hvis resultatet av and er #t (altsaa #t*#t=#t) returneres
;; siste uttryk, -1, 0 eller 1. (tallet er ikke bolsk og evalueres
;; til #t som standard for logiske uttrykk.)
;; - i neste omgang returneres foerste hendelse som evalueres til
;; true av or operatoren.

(sign3 0)


;;--------- OPPGAVE 3 ----------

;;-- 3A
(define add1
  (lambda (a)
    (+ a 1)))

(define sub1
  (lambda (b)
    (- b 1)))

(add1 3)
;; => 4
(sub1 2)
;; => 1
(add1 (sub1 0))
;; => 0


;;-- 3B
;; Rekursiv prosedyre for addisjon.

(define plus
  (lambda (x y)
    (if (zero? x)
	y
	(add1 (plus (sub1 x) y)))))
;; plus(3 4). x != 0:
;; (add1 (plus 2 4)). x != 0:
;; (add1 (add1 (plus 1 4). x != 0:
;; (add1 (add1 (add1 (plus 0 4). x == 0:
;; (add1 (add1 (add1 4)
;; (add1 (add1 5)
;; (add1 (6))
;; 7

(plus 3 4)

;;-- 3C
;; Iterative prosedyre:
;; Legger til og trekker fra helt til y evalueres
;; til null. Returnerer da x.
(define plus
  (lambda (x y)
    (if (zero? y)
	x
	(plus (add1 x) (sub1 y)))))

;; Eks: (pluss 1 2)
;; plus(1 2). y != 0, kaller seg selv med nye parametere:
;; plus(2 1). y != 0, kaller seg selv:
;; plus(3 0). y == 0, returnerer:
;; 3
(plus 1 2) ;;=> 3


;;-- 3D
;; Konseptet i scheme (Lisp) er vel aa infoere en
;; abstraksjon til prosedyrene, saakalt black-boxing.
;; Det man oensker er aa "bake" alle hjelpe-prosedyrene inn
;; i "boksen" begrenset av ytterste parentes-par.
;; Konsekvensen av dette er at alt som er definert, samt
;; argumentene er interne i "boksen".
;;
;; Selve abstraksjonen viser seg ved at bruker kun trenger aa
;; aa vaere kjent med operatoren og benytter seg av denne,
;; og ikke den interne algoritmen for beregningen,
;; da det ikke er interessant (sett fra brukers perspektiv).


(define (power-close-to b n)
  (define (power-iter e)
    (if (> (expt b e) n)
	e
	(power-iter (+ 1 e))))
  (power-iter 1))

(power-close-to 2 8)

    
