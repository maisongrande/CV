#######################################################################
# Navn: itoarr
#
# Forfatter:
#   Christopher Storheim
#
# c-signatur:
#   int itoa(char *str, int n)
# Synopsis:
#   Konverterer heltall (int) til char.
# stakk:
#	tallet som skal konverteres ligger 12 bytes over base ptr.
#	peker til char array ligger 8 bytes over base ptr.
#
# Registre:
#   edi = peker til array, (edi) = inneholdet i arrayet.
#   eax = n
#   ecx = teller, oeker hver gang en ny char skrives til edi.
#
# ASCII tegn: '-' = 0x2d, '0' = 0x30 
#######################################################################
.globl _itoarr

_itoarr:
pushl	%ebp
movl	%esp,		%ebp            # Standard funk start. sp->bp
movl	8(%ebp),	%edi            # Henter *str
movl	12(%ebp),	%eax            # Henter n
movl    $0x00,      %ecx            # Initialiser teller
# Initial TEST:
cmpl	$0x00,		%eax	        # Sjekk om tallet er negativt
jge	pos_tall                        # Hvis >= hopp til p_t, ellers:
movb	$0x2d,		(%edi, %ecx, 1) # Legg inn minus-tegn i str[0]
incl    %ecx                        # Ny char, teller++
imull	$-0x01,		%eax		    # Gjoer tallet positivt.


pos_tall:                           # TEST FERDIG:
xorl    %edx,       %edx            # Null ut rest-reg
movl    $10,        %esi            # Forbered div 10
divl    %esi                        # div tall med 10, rest i edx

addl    $0x30,      %edx            # lst ligger i edx, +ascii(0)
movb    %dl,        (%edi, %ecx, 1) # legg dette tallet i array
incl    %ecx                        # nytt tall, teller++
cmpl    $0x0,       %eax            # Test om det er flere siffer
jnz     pos_tall                    # Hvis ikke repeter.


return:
.align                              # Noedvendig?
movb    $0x00,      (%edi, %ecx, 1) # Pad med null-byte
movl    %ecx,       %eax            # Strlen -> returverdi
popl    %ebp                        # Reset frame
ret                                 # Returner til kaller.
