/**
 * programme quick and dirty pour convertir une image contenant des dessins de lettres
 * en version exploitable en C
 * source :
 * - un fichier texte contenant les caractères à décrire : une ligne par ligne dans l'image,
 *   et un caractère par caractère dans l'image
 * - un fichier png contenant l'image de chaque de caractère, avec un pixel de séparation entre chaque
 * - une largeur et une hauteur en pixel, de chaque image
 * donc, s'il y a W caractères par ligne, et H lignes, et qu'on annonce w par h pixels, l'image doit faire
 * PW=W*(w+1)+1 par PH=H*(h+1)+1
 */

// TODO : générer un typedef ou un define du type de la table
// = char si h <=8, int sinon

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

//#define PNG_DEBUG 3
#include "libpng15/png.h"

void usage(char *cmd) {
	fprintf(stderr, "Usage : %s [-r] txt_filename png_filename char_width char_height\n", cmd);
	exit(1);
}

int w, h, W, H, PW, PH;
char reverse = 0;
char *txt, *png;

char **lines;
int nb_lines, *line_lengths, max_line_length, nb_chars;

png_structp png_ptr;
png_infop info_ptr;
int depth;
png_bytep * row_pointers;

/* directement repris de http://zarb.org/~gc/html/libpng.html */
void abort_(const char * s, ...)
{
        va_list args;
        va_start(args, s);
        vfprintf(stderr, s, args);
        fprintf(stderr, "\n");
        va_end(args);
        abort();
}

void read_png_file(char* file_name)
{
    unsigned char header[8];    // 8 is the maximum size that can be checked
    int y;

    /* open file and test for it being a png */
    FILE *fp = fopen(file_name, "rb");
    if (!fp)
        abort_("[read_png_file] File %s could not be opened for reading", file_name);
    fread(header, 1, 8, fp);
    if (png_sig_cmp(header, 0, 8))
        abort_("[read_png_file] File %s is not recognized as a PNG file", file_name);


    /* initialize stuff */
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!png_ptr)
        abort_("[read_png_file] png_create_read_struct failed");

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
        abort_("[read_png_file] png_create_info_struct failed");

    if (setjmp(png_jmpbuf(png_ptr)))
        abort_("[read_png_file] Error during init_io");

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);

    png_read_info(png_ptr, info_ptr);

    PW = png_get_image_width(png_ptr, info_ptr);
    PH = png_get_image_height(png_ptr, info_ptr);

    png_read_update_info(png_ptr, info_ptr);

    /* read file */
    if (setjmp(png_jmpbuf(png_ptr)))
        abort_("[read_png_file] Error during read_image");

    row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * PH);
    for (y=0; y<PH; y++)
        row_pointers[y] = (png_byte*) malloc(png_get_rowbytes(png_ptr,info_ptr));

    png_read_image(png_ptr, row_pointers);

    fclose(fp);
}

void read_txt_file(char *file_name) {
	int lll= 10;
	int i= 0;
	
	max_line_length= 0;
	nb_chars= 0;

	/* TODO : verifier que tous les malloc/realloc passent bien */
	line_lengths= (int *)malloc(lll*sizeof(int));
	lines= (char **)malloc(lll*sizeof(char*));

	FILE *fp= fopen(file_name, "rb");
	while(!feof(fp)) {
		if(i>=lll) {
			lll=i+1;
			line_lengths= (int *)realloc(line_lengths, lll*sizeof(int));
			lines= (char **)realloc(lines, lll*sizeof(char*));
		}
		lines[i]= (char *)malloc(100); /* on limite arbitrairement a 100. */
		fgets(lines[i], 100, fp);
		line_lengths[i]= strlen(lines[i]);
		/* virer le \n final */
		if(lines[i][line_lengths[i]-1]=='\n') {
			line_lengths[i]--;
			lines[i][line_lengths[i]]='\0';
		}
		if(line_lengths[i]==0) {
			break;
		}
		if(max_line_length<line_lengths[i]) {
			max_line_length= line_lengths[i];
		}
		nb_chars+= line_lengths[i];
		i++;
	}
	H=nb_lines= i;
	W=max_line_length;

	/* ICI : on a un tableu de lignes avec tous les caractères,
	 * et un autre precisant combien de caractère on a par ligne
	 * de plus, max_line_length contient la taille de la plus grande ligne
	 * enfin, nb_lines contient le nombre de lignes
	 * => on peut en déduire la taille que doit faire l'image
	 */
}

void write_output() {
	int i, x, y;
	char *c;

	printf("/** conversion from %s / %s , chars of %dx%d pixels **/\n", txt, png, w, h);
	if(reverse) {
		printf("/** in reversed mode **/\n");
	}
	printf("/** %d chars to convert in %d lines of up to %d chars**/\n\n", nb_chars, nb_lines, max_line_length);

	printf("#define NB_CHARS (%d)\n", nb_chars);
	printf("#define CHAR_WIDTH (%d)\n", w);
	printf("#define CHAR_HEIGHT (%d)\n", h);
	printf("#define MAPS_NB_BYTES_PER_CHAR (%d)\n\n", (h+7) / 8 * w);
	printf("#define MAPS_NB_BYTES (MAPS_NB_BYTES_PER_CHAR*NB_CHARS)\n\n");

	printf("#define MAP_FOR(i) (&(MAPS[(i)*MAPS_NB_BYTES_PER_CHAR]))\n\n");
	
	printf("char *CHARS=");

	for(i=0; i<nb_lines; i++) {
		printf("\n  \"");
		c= lines[i];
		while(*c) {
			if(*c=='\\' || *c=='"') {
				putchar('\\');
			}
			putchar(*c);
			c++;
		}
		printf("\"");
	}
	printf(";\n\n");

	printf("unsigned char MAPS[MAPS_NB_BYTES]={");

	for(y=0; y<nb_lines; y++) {
		c= lines[y];
		x=0;
		while(*c) {
			/* find *c description in png */
			int x0= 1+x*(w+1), xx;
			int y0= 1+y*(h+1), yy;
			unsigned char msk, v;

			printf("\n  /* %c */ ", *c);

			for(xx=x0; xx<x0+w; xx++) {
				v=0; msk= reverse ? 0x01 : 0x80;
				yy=y0;
				for(;;) {
					if((row_pointers[yy][xx*depth] & 1) == 0) {
						v|=msk;
					}
					if (reverse) {
						msk<<=1;
					} else {
						msk>>=1;
					}
					yy++;
					if(msk==0 || yy==y0+h) {
						printf(" 0x%02x,", v);
						msk= 0x80; v=0;
					}
					if(yy==y0+h) break;
				}
			}
			c++; x++;
		}
	}

	printf("\n};\n");
}

int main(int argc, char **argv) {

	if(argc!=5 && argc!=6) {
		usage(argv[0]);
	}
	if (argv[1] && strcmp(argv[1], "-r") == 0) {
		reverse = 1;
		argv++; argc--;
	}
	txt= argv[1];
	png= argv[2];
	w=atoi(argv[3]);
	h=atoi(argv[4]);

	if(w<=0 || h<=0) {
		fprintf(stderr, "can't convert width/height\n");
		usage(argv[0]);
	}

	read_txt_file(txt);
	read_png_file(png);

	if(PW!=W*(w+1)+1 || PH!=H*(h+1)+1) {
		fprintf(stderr, "incoherencies between sizes %d!=%d*(%d+1)+1 || %d!=%d*(%d+1)+1\n",
				PW, W, w, PH, H, h);
		exit(1);
	}
    switch(png_get_color_type(png_ptr, info_ptr)) {
	    // selon type, decider nb octets/pixel et valeurs blanc/noir ?
	    case PNG_COLOR_TYPE_PALETTE: /* 1 ? */
	    case PNG_COLOR_TYPE_GRAY:    /* 1 ? */
	    	depth=1;
	    	break;
	    case PNG_COLOR_TYPE_GA:      /* 2 ? */
	    	depth=2;
	    	break;
	    case PNG_COLOR_TYPE_RGB:     /* 3 ? */
	    	depth=3;
	    	break;
	    case PNG_COLOR_TYPE_RGBA:    /* 4 ? */
	    	depth=4;
	    	break;
	    default:
			fprintf(stderr, "unknonw color type\n");
			exit(1);
    }

    /***
    for (y=0; y<PH; y++) {
        png_byte* row = row_pointers[y];
        for (x=0; x<PW; x++) {
            png_byte* ptr = &(row[x*depth]);
            putchar(*ptr & 1 ? ' ' : '*');
        }
        printf("\n");
    }
    ***/

	write_output();

	return 0;
}
