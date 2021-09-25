###############################################################
#
#   C - library for Oblig 3.
#
###############################################################


rgbTohsi = r'''

    int i; 
    int j;
    int k;
    int it;
    float min = 0;
    unsigned int r,g,b;
    float tmp = 0;
    float I = 0, S = 0, H = 0;
    const float pi = 3.14159256;
    const float deg = pi/180;


    for (i = 0; i < n; i++){
        for (j = 0; j < m; j++){

            for (k = 0; k < 3; k++){
            
                switch (k){
                    case 0:
                        if (uR == 0){	
                            r = V3(i, j, k);
                        } else {
                            r = V3(i, j, k) + uR;
                            r = fmod(r,256);
                        }
                        break;

                    case 1:	
                        if (uG == 0){
                            g = V3(i, j, k);
                        } else {
                            g = V3(i, j, k) + uG; 
                            g = fmod(g,256);
                        }
                        break;

                    case 2:	
                        if (uB == 0){
                            b = V3(i, j, k);
                        } else {
                            b = V3(i, j, k) + uB;
                            b = fmod(b,256);
                        }
                        break;

                    default:	
                        printf("SWITCH Ghosts Ahoi!");	
                        exit(0);
                        break;
                } /* end switch */
            } /* end for */


        tmp = acos((r-g*0.5-b*0.5)/ sqrt(r*r+g*g+b*b-r*g-r*b-g*b));
        I = (r+g+b) / 3.0;

        min = ((r<=b)?((r<=g)?r:g):((r<=b)?r:b));

        S = (I > 0)?(1-(min/I)):0;
        
        if ( r == b && b == g){
            H = 0;
        } else {
            H = (g >= b) ? ((180*tmp)/pi):(360-((180*tmp)/pi));
        }

            for (k = 0; k < 3; k++){
            
                switch (k){

                    case 0:	
                        V3(i, j, k) = H;
                        break;

                   case 1:	
                        V3(i, j, k) = S;
                        break;

                    case 2:	
                        V3(i, j, k) = I;
                        break;

                    default:	
                        printf("SWITCH Ghosts Ahoi!");	
                        exit(0);
                        break;

                } /* end switch */
            }
        }
    }
'''

##--------------------------------- End rgb2hui ------#

hsiToRgb =r'''

    int i; 
    int j;
    int k;
    unsigned int r = 0,g = 0,b = 0;
    float I = 0, S = 0, H = 0;
    const float pi = 3.14159256;
    const float deg = pi/180;


    for (i = 0; i < n; i++){
        for (j = 0; j < m; j++){
    
            for (k = 0; k < 3; k++){
                    switch (k){
                        case 0:	
                            if (uH == 0){
                                H = V3(i, j, k);
                            } else {
                                H = V3(i, j, k) + uH;
                                H = fmod(H,360);
                            } 
                            break;

                        case 1:	
                            if (uS == 0){
                                S = V3(i, j, k);
                            } else {
                                S = V3(i, j, k) + uS;
                                S = fmod(S,360);
                            }
                            break;

                        case 2:	
                            if (uI == 0){
                                I = V3(i, j, k);
                            } else {
                                I = V3(i, j, k) + uI;
                                I = fmod(I,360);
                            }
                            break;

                        default:	
                            printf("SWITCH Ghosts Ahoi!");	
                            exit(0);
                            break;
                    } /* end switch */
            } /* end for */
            


            if (H >= 360 || H < 0){
                H = fmod(H,360);
            } else if (H == 0){	

                r = I + 2*I*S;
                g = I - I*S;
                b = I - I*S;

            } else if ( H > 0 && H < 120){

                r = I + I*S*(cos(H*deg)/cos((60-H)*deg));
                g = I + I*S*(1-(cos(H*deg)/cos((60-H)*deg)));
                b = I - I*S;

            } else if(H == 120.0){

                r = I - I*S;
                g = I + 2*I*S;
                b = I - I*S;

            } else if (H > 120 && H < 240){

                r = I - I*S;
                g = I + I*S*(cos((H-120)*deg)/cos((180-H)*deg));
                b = I + I*S*(1 - (cos((H-120)*deg)/cos((180-H)*deg)));

            } else if (H == 240){

                r = I - I*S;
                g = I - I*S;
                b = I + 2*I*S;

            } else if (H > 240 && H < 360){	

                r = I + I*S*(1-(cos((H-240)*deg)/cos((300-H)*deg)));
                g = I - I*S;
                b = I + I*S*(cos((H-240)*deg)/cos((300-H)*deg));

            } else {
                printf("IF-Else Ghosts Ahoi!");	
                
            } /* end if else */
                        
                for (k = 0; k < 3; k++){
                
                    switch (k){
                        case 0:	
                            V3(i, j, k) = r;
                            break;

                       case 1:	
                            V3(i, j, k) = g;
                            break;

                        case 2:	
                            V3(i, j, k) = b;
                            break;

                        default:	
                            printf("SWITCH Ghosts Ahoi!");	
                            exit(0);
                            break;

                    } /* end switch */
                }
            }
        }
'''
##--------------------------------- End hui2rgb ------#

dNoisebw = r'''

        int i; 
        int j;
        int iter;

        for (i = 0; i < m; i++){
            V2(i, 0) = U2(i, 0);
            V2(i, n-1) = U2(i, n-1);
        }

        for (i = 1; i < n-1; i++){
            V2(0, i) = U2(0, i);
            V2(m-1, i) = U2(m-1, i);
        }

        for (iter = 0; iter < iters; iter++){

            for (i = 1; i < n -1; i++){

                for (j = 1; j < m-1; j++){

                    V2(i, j) = (U2(i, j) + 
                                kappa*(U2(i-1, j) + 
                                U2(i, j-1) - 
                                U2(i, j)*4 + 
                                U2(i, j+1) + 
                                U2(i+1, j)));
                }
            }

            t = u;
            u = v;
            v = t;
        }
'''

##--------------------------------- End dNoisebw ------#

dNoiseClr = r'''

        int i; 
        int j;
        int rgb;
        int iter;

        for (i = 0; i < n; i++){
            for (rgb = 0; rgb < 3; rgb++){
                V3(i, 0, rgb) = IMGDAT3(i, 0, rgb);
                V3(i, m-1, rgb) = IMGDAT3(i, m-1, rgb);
            }
        }

        for (i = 1; i < m-1; i++){
            for (rgb = 0; rgb < 3; rgb++){
                V3(0, i, rgb) = IMGDAT3(0, i, rgb);
                V3(n-1, i, rgb) = IMGDAT3(n-1, i, rgb);
            }
        }
        for (iter = 0; iter < iters; iter++){
            for (i = 1; i < n -1; i++){
                for (j = 1; j < m-1; j++){
                    for (rgb = 0; rgb < 3; rgb++){
                        V3(i, j, rgb) = (IMGDAT3(i, j, rgb) + 
                                        kappa*( IMGDAT3(i-1, j, rgb) + 
                                        IMGDAT3(i, j-1, rgb) - 
                                        IMGDAT3(i, j, rgb)*4 + 
                                        IMGDAT3(i, j+1, rgb) + 
                                        IMGDAT3(i+1, j, rgb)));
                    }
                }   
            }
            t = imgdat;
            imgdat = v;
            v = t;
        }
'''
##--------------------------------- End dNenoiseClr ------#
