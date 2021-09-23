        #########################################################################
        ##                                                                     ##
        ##				Oblig 3                                ##
        ##                              INF2270                                ##
        ##                        Christopher Storheim                         ##
        ##                                                                     ##
        #########################################################################


        # Lokale variable:
        .data

        # Navngir konstanter:
        PERCENTCHAR:    .byte 0x25              # '%'  -> HEX-verdi
        NULLCHAR:       .byte 0x00              # '\0' -> HEX-verdi
        FLOATTYPE:      .byte 0x66              # 'f'  -> HEX-verdi
        INTTYPE:        .byte 0x64              # 'd'  -> HEX-verdi
        STRINGTYPE:     .byte 0x73              # 's'  -> HEX-verdi
        CHARTYPE:       .byte 0x63              # 'c'  -> HEX-verdi
        HEXTYPE:        .byte 0x78              # 'x'  -> HEX-verdi

        
        # tokens: Oppslag for korresponderende ascii-verdi ifht index.
        tokens: .ascii  "0123456789abcdef"


        feilMld: .ascii "N/A"
        
        .align 2

        # loc_store: Lokal mellomlagring av data. Tenkt som buffer for
        # aa kunne reverse strenger.
        loc_store:      .long 128


        # Program:
        .text
        .globl _sprinter

        # Navn:         Sprinter
        #
        # c-signatur:   sprinter(char *str, const char * format, ...)
        #
        # Synopsis:     Skriver format-str fra src over i dst
        #
        # Forklaring:
        #
        # Sprinter tar inn et variablet antall parametere. Ved kall pa sprinter
        # fra eksternt program, vil den eksterne kalleren pushe alle
        # parametere pa stacken i baklengs rekkefoelge.
        # kall-stakken initielt:
        #       ..
        #		|        arg-n       |
        #       ..      +--------------------+ 
        #               |         ...        |
        #	20	+--------------------+
        #		|        arg1        |
        #	16	+--------------------+
        #		| const char *format |
        #	12	+--------------------+
        #		|      char *str     |
        #	8	+--------------------+
        #		|      ret adr.      |
        #       4       +--------------------+
        #               |      Gml. EBP      |
        #       0       +--------------------+ <-ESP <-EBP
        #		|                    |
        #       -4      +--------------------+
        #		|                    |
        #       ..      +--------------------+ 
        #               |                    |
        # Maaten jeg finner ut av hvor mange parametere det er pushet pa stakken
        # er a telle antall %-tegn. Hvert %-tegn tilsvarer 2 words opp paa
        # stakken.
        #
        # Registere:
        #
        # Caller Save Registers:
        #               EAX,    EDX,    ECX
        # Callee Save Registers:
        #               EBX,    ESI,    EDI
        #
        # EDI: Peker til adresser av char *str.
        # ESI: Peker til const char* format_str
        # ECX: StrLen
        # EDX: ARGV ref.
        # EAX: Fri bruk


_sprinter:
        # Hent char *format str:
        # -Let gjennom strengen etter %-tegn
        # -Hvis ingen prosenttegn str == \0:
        #       put str i *str. og returner.
        # -Hvis % tegn og streng != \0:
        #       Teller++
        # -Naar ferdig:
        #	Legg til null-byte til array.
        #	returner.

        #########################################
        #             INITIALISER               #
        #########################################

        # Funskjonsstart (Standard):
        pushl	%ebp                            # Lagre basepointer.
        movl	%esp,		%ebp            # Lagre stackpointer.

        # Lag stakk og sett EBX: stakk-peker
        pushl   %ebx                            # Wishfull-Thinking:
        subl    $20,            %esp            # Lokalt lagringsomrd. for str.
        leal    -20(%ebp),      %ebx            # bind EBX til addr.

        # Henter *format string og *str:
        movl	12(%ebp),	%esi            # 12*dwords relativt til baseptr
        movl    8(%ebp),        %edi            # Hent ref til char str[]

        # Nuller ut tellere, EDX: argc, ECX: strlen
        xorl    %ecx,		%ecx            # count/ptr til 0 og hent str:
        xorl    %edx,           %edx            # 12 + 4, 8, 12, 16 ifht ebp




        #########################################
        #      LOP GJENNOM FORMATSTRENGEN:      #
        #########################################
search_format_str:
        # Denne prosedyren loeper gjennom strengen og skriver inn i det
        # lokale arrayet frem til og etter %-tegn blir funnet eller strengen
        # er lopet ut (char == '\0').

        lodsb                                   # Hent char ecx * b fra indx 0.
        # char = '\0'                           # IF:
        cmpb	NULLCHAR,       %al             # Base case: test str=tom/slutt
        je	done_str                        # Hvis saa hopp til ferdig
        # char =  '%'                           # ELSE IF:
        cmpb	PERCENTCHAR,	%al             # Test om %-tegn
        je	symb_found                      # Fant en?, hopp til subrutine.

        # put char                              # ELSE:
        stosb                                   # Legg ikke-match i array
        incl     %ecx                           # char-peker++
        jmp search_format_str                   # Gjenta til null-term




        #########################################
        #          TREFF PA PROSENTTEGN.        #
        #          FINN HVILKEN DATATYPE:       #
        #########################################
symb_found:
        # Naa er et %-tegn funnet. Det som maa gjoer naa er aa gaa til neste
        # char i strengen og teste hvilken datatype som skjuler seg pa stakken.
        # Hvis neste er lik %:
        #	substituer med 0x23 (des.: 35)
        # Ellers sjekk om neste er lik:
        #	d -> konverter int til char og substituer %-d med resultatet.
        #	f -> konverter float til char og substituer.
        #	x -> konverter XXX til hex og substituer.
        #	s -> sett inn strengen for symbolet %-s
        #	c -> erstatt med character.

        incl    %edx                            # %-teller++
        lodsb                                   # Hent neste char over i EAX

        # Switch(al):

        #---case c:
        cmpb	CHARTYPE,       %al             # test 'c'
        je      add_char

        #---case d:
        cmpb    INTTYPE,        %al             # test 'd'
        je      add_int

        #case---f:
        cmpb    FLOATTYPE,      %al             # test 'f'
        je      add_float

        #case---s:
        cmpb    STRINGTYPE,     %al             # test 's'
        je      add_str

        #case---x:
        cmpb    HEXTYPE,        %al             # test 'x'
        je      add_hex

        #case---%:
        cmpb    PERCENTCHAR,    %al             # test '%'
        je      add_percent

        # Fell through:                         # Ingen kjente typer?
        push    %esi
        pushl   %ecx    
        movl    $3,             %ecx            
        leal    feilMld,        %esi            # Informer bruker
        rep     movsb
        popl    %ecx
        popl    %esi
        jmp     break


        #########################################
        #            ADD CHAR TO ARRAY          #
        #########################################
add_char:
        movl    12(%ebp,%edx,4),%eax            # Hent argv[(4*edx + 12)(ebp)]
        stosb                                   # skriv funnet char
        incl    %ecx
        jmp     break                           # returner.




        #########################################
        #              ADD % TO ARRAY           #
        #########################################
add_percent:
        # Legger ett %-tegn som erstatning for foerste %-tegn.
        # Kunne sikkert skippet og flyttet peker, men like greit slik.

        movl    PERCENTCHAR,    %eax            # Klargjoer et %-tegn
        stosb                                   # skriv til **str
        incl    %ecx                            # Ny char, teller++
        decl    %edx                            # Mistenker at vi er innom her
        jmp break                               # litt for ofte....




        #########################################
        #            ADD INT TO ARRAY           #
        #########################################
add_int:
        # Kaller prosedyren int_to_str og kanskje add_str etterpa? Hvis jeg
        # Klarer aa huske aa padde nummeret med null-byte
        pushl   %edx
        pushl   %esi

        movl    12(%ebp,%edx,4),%eax            # Hent argv[(4*edx + 12)(ebp)]

        cmpl	$0x00,		%eax		# Sjekk om tallet er negativt
        jge	proc_tall                       # Hvis >= hopp til p_t, ellers:
        pushl   %eax
        movl	$0x2d,		%eax            # Legg inn minus-tegn i str[0]
        stosb
        incl    %ecx                            # Ny char, teller++
        popl    %eax
        imull	$-0x01,		%eax		# Gjoer tallet positivt.

proc_tall:                                      # TEST FERDIG:
        pushl   %ecx                            # Lagre strlen
        xorl    %ecx,           %ecx            # Null ut for denne prosedyren
        pushl   %edi
        leal    loc_store,      %edi            # Faa peker til array

itoc:
        xorl    %edx,           %edx            # Null ut rest-reg
        movl    $10,            %esi            # Forbered div 10
        divl    %esi                            # div tall med 10, rest i edx
        addl    $0x30,          %edx            # Minst signif ligger i edx:
                                                # + '\0' for aa faa ascii-repr
        pushl   %eax                            # Tom for reg.
        movb    %dl,            %al             # klargjoer for skriving
        stosb                                   # Skriv  tallet til dest
        popl    %eax                            # gjennopprett reg
        incl    %ecx                            # nytt tall, teller++
        cmpl    $0x0,           %eax            # Test om det er flere siffer
        jnz     itoc                            # Hvis saa,  repeter.
        movl    %ecx,           %ebx            # Ta vare paa antallet konv.
        leal    -1(%edi),       %esi            # peker paa siste tall i array.
                                                # Minus en fordi teller++
        popl    %edi                            # Restore peker til char *str

rev_loop:
        cmpl    $0x0,            %ecx           # for alle skrevne tall
        je      done_rev
        std                                     # rev peker
        lodsb                                   # legg inneh. i AL
        cld                                     # snu igjen
        stosb                                   # skriv char til **str
        decl    %ecx
        jmp     rev_loop

done_rev:
        popl    %ecx                            # hent gammel strlen
        addl    %ebx,           %ecx            # legg til nytt ant
        popl    %esi                            # gjennopprett stakk
        popl    %edx

        jmp break                               # Ferdig




        #########################################
        #         ADD DOUBLE TO ARRAY           #
        #########################################
add_float:
        # Bruker utlevert kode for aa konvertere float til char
        # signatur my_ftoa(char* dst, double n).

        pushl   %esi
        movl    12(%ebp,%edx,4),%ebx            # Hent 32 bit av dbl
        incl    %edx                            # arg-ptr++
        pushl   %eax                            # skaff ledig reg
        movl    12(%ebp,%edx,4),%eax            # hent resten
        pushl   %edx                            # Registerene kommer til aa bli
        pushl   %ecx                            # herjet med saa redd de som
        pushl   %edi                            # reddes kan
        
        leal    loc_store,      %edi            # Skaff ref til denne str inn
        leal    loc_store,      %esi            # og siden ut.

        pushl   %eax                            # Argumentstakken:
        pushl   %ebx
        pushl   %edi

        call   _my_ftoa                         # kall funksjonen

        popl    %edi                            # Frigi registrene
        popl    %ebx
        popl    %eax
        popl    %edi
        popl    %ecx                            # teller gjennopprettet
        
loop_double:                                    # Loep gjennom det funksjonen
        lodsb                                   # produserte og skriv dette
        cmpb    $0,             %al             # til str[ECX]
        je      done_double
        stosb
	incl    %ecx            
        jmp     loop_double
        
done_double:                                    # Ferdig, frigi resten
        popl    %edx            
        popl    %eax
        popl    %esi
        jmp break                               # returner.




        #########################################
        #         ADD STRING TO ARRAY           #
        #########################################
add_str:
        # Treff pa %s indikerer at argument-n er en peker til streng.
        # Da pusher jeg formatstrengen pa stakken og bytter peker fra den
        # til den nye. Loeper sa gjennom denne og kopierer den over til array.

        pushl   %esi                            # Ta vare paa peker til f-str
        movl    12(%ebp,%edx,4),%esi            # Hent argv[(4*edx + 12)(ebp)]
while_not_null:
        lodsb                                   # Henter en char inn i AL
        cmpb    NULLCHAR,       %al             # Frem til null term:
        je      exit_add_str
        stosb                                   # Legger til ny char fra AL
        incl    %ecx                            # strlen++
        jmp while_not_null
exit_add_str:
        popl    %esi                            # Hent gammel ref
        jmp     break                           # ferdig!




        #########################################
        #            ADD HEX TO ARRAY           #
        #########################################
add_hex:
        # Treff pa %->x indikerer at tallet skal gjoeres om til hexadesimalt
        # Jeg har tenkt at a loepe gjennom 4-byte strengen i 8 steg og
        # rightskifte ut 4 bit ved hver gjennomgang, det burde gi riktig
        # resultat. Sjekker saa hver verdi opp mot en teller, og denne
        # telleren brukes til oppslag i tokens-tabellen.

        # Init-test: Register er null:
        pushl   %esi                            # Lagre variabler
        movl    12(%ebp,%edx,4),%esi            # Hent tall-verdi
        testl   %esi,           %esi            # Verdien til tallet null?
        jnz     hex_gtZero                      # Skip resten
        movb    $0x30,          %al             # Skriv null, strlen++ og retur
        stosb
        incl    %ecx
        popl    %esi
        jmp     break                           # Ferdig

hex_gtZero:
        pushl   %ebx                            # Lagre registere
        pushl   %edx
        pushl   %ecx
        xorl    %edx,           %edx            # Null ut registere
        xorl    %ebx,           %ebx
        xorl    %ecx,           %ecx
        pushl   %edi
        leal    loc_store,      %edi

shift_loop:
        movl    $0xf,           %edx

cmp_loop:                                       # Loop over alle bit i byte:
        andl    %esi,           %edx
        cmpb    %dl,            %bl             # match?
        je      found_hex                       # ikke:
        incl    %ebx                            # incr. teller
        cmpb    $0xf,           %bl
        ja      done_hex
        jmp     cmp_loop                        # ellers 15 <-| cmp_loop

found_hex:                                      # Match pa tall, ligger i EBX:
        movb    tokens(%ebx),   %al             # hent ascii pa plass EBX
        stosb                                   # Print dette tallet.
        incl    %ecx                            # strlen++
        shr     $4,             %esi            # shift ut fire bit
        cmpl    $0,             %esi            # Sjekk om ferdig.
        je      done_hex                        # Ellers:
        
        xorl    %ebx,           %ebx            # reset teller
        jmp     shift_loop                      # og gjenta

done_hex:
        movl    %ecx,           %ebx            # Ta vare paa antallet konv.
        leal    -1(%edi),       %esi            # peker paa siste tall i array.
                                                # Minus en fordi teller++
        popl    %edi                            # Restore peker til char *str

rev_loop_hex:
        cmpl    $0x0,           %ecx            # for alle skrevne tall
        je      return_hex
        std                                     # rev peker
        lodsb                                   # legg inneh. i AL
        cld                                     # snu igjen
        stosb                                   # skriv char til **str
        decl    %ecx
        jmp     rev_loop_hex

return_hex:
        popl    %ecx                            # hent tidl strlen
        addl    %ebx,           %ecx            # legg til ny
        popl    %edx
        popl    %ebx                            # Rett opp stacken
        popl    %esi

        jmp     break                           # Ferdig




        #########################################
        #           BREAK: SWITCH/CASE          #
        #########################################
break:
        # Default for alle sammenligningene.
        # Prosedyren hopper tilbake til streng-populeringen.
        jmp	search_format_str





        #########################################
        #      KLARGJOER RETUR FRA FUNKSJON     #
        #########################################
done_str:
        # Her avsluttes strengen. Lagt utenfor return for ryddighet
        # og mer oversiktlig kode.
        movb    NULLCHAR,           %al             # Terminer array
        stosb




        #########################################
        #              EXIT_SUCCESS             #
        #########################################
return:
        # Funksjon retur (standard)
        movl    %ecx,           %eax            # strlen -> ret:EAX
        addl    $20,            %esp            # Gjennopprett stakk
        popl    %ebx                            #
        popl	%ebp                            #
        ret                                     # Returner.




#******************************************************************************#
#******************    DONE SPRINTER    ***************************************#
#******************************************************************************#
