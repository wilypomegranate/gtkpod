/*
|  Changed by Jorg Schuler <jcsjcs at users.sourceforge.net> to
|  compile "standalone" with the gtkpod project. 2003/04/01
*/

/* This code is taken from the mpg123 code used in xmms-1.2.7
 * (Input/mpg123). Only the code needed for the playlength calculation
 * has been extracted */

#include "mp3file.h"
#include <stdio.h>
#include <string.h>

#define FRAMES_FLAG     0x0001
#define BYTES_FLAG      0x0002
#define TOC_FLAG        0x0004
#define VBR_SCALE_FLAG  0x0008

#define         SBLIMIT                 32
#define         SCALE_BLOCK             12
#define         SSLIMIT                 18

#define         MPG_MD_STEREO           0
#define         MPG_MD_JOINT_STEREO     1
#define         MPG_MD_DUAL_CHANNEL     2
#define         MPG_MD_MONO             3
#define MAXFRAMESIZE 1792
#define real float

struct bitstream_info
{
	int bitindex;
	unsigned char *wordpointer;
};

struct bitstream_info bsi;

real mpg123_muls[27][64];	/* also used by layer 1 */

int tabsel_123[2][3][16] =
{
	{
    {0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448,},
       {0, 32, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 384,},
       {0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320,}},

	{
       {0, 32, 48, 56, 64, 80, 96, 112, 128, 144, 160, 176, 192, 224, 256,},
	    {0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160,},
	    {0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160,}}
};

long mpg123_freqs[9] =
{44100, 48000, 32000, 22050, 24000, 16000, 11025, 12000, 8000};

/*
 * structure to receive extracted header
 */ 
typedef struct
{
	int frames;		/* total bit stream frames from Xing header data */
	int bytes;		/* total bit stream bytes from Xing header data */
	unsigned char toc[100];	/* "table of contents" */
} xing_header_t;

struct al_table
{
	short bits;
	short d;
};

struct frame
{
	struct al_table *alloc;
	int (*synth) (real *, int, unsigned char *, int *);
	int (*synth_mono) (real *, unsigned char *, int *);
#ifdef USE_3DNOW
	void (*dct36)(real *,real *,real *,real *,real *);
#endif
	int stereo;
	int jsbound;
	int single;
	int II_sblimit;
	int down_sample_sblimit;
	int lsf;
	int mpeg25;
	int down_sample;
	int header_change;
	int lay;
	int (*do_layer) (struct frame * fr);
	int error_protection;
	int bitrate_index;
	int sampling_frequency;
	int padding;
	int extension;
	int mode;
	int mode_ext;
	int copyright;
	int original;
	int emphasis;
	int framesize;		/* computed framesize */
};

static guint32 convert_to_header(guint8 * buf)
{

	return (buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + buf[3];
}

static int mpg123_head_check(unsigned long head)
{
	if ((head & 0xffe00000) != 0xffe00000)
		return FALSE;
	if (!((head >> 17) & 3))
		return FALSE;
	if (((head >> 12) & 0xf) == 0xf)
		return FALSE;
	if (!((head >> 12) & 0xf))
		return FALSE;
	if (((head >> 10) & 0x3) == 0x3)
		return FALSE;
	if (((head >> 19) & 1) == 1 && ((head >> 17) & 3) == 3 && ((head >> 16) & 1) == 1)
		return FALSE;
	if ((head & 0xffff0000) == 0xfffe0000)
		return FALSE;
	
	return TRUE;
}


/*
 * the code a header and write the information
 * into the frame structure
 */
static int mpg123_decode_header(struct frame *fr, unsigned long newhead)
{
    int ssize;

	if (newhead & (1 << 20))
	{
		fr->lsf = (newhead & (1 << 19)) ? 0x0 : 0x1;
		fr->mpeg25 = 0;
	}
	else
	{
		fr->lsf = 1;
		fr->mpeg25 = 1;
	}
	fr->lay = 4 - ((newhead >> 17) & 3);
	if (fr->mpeg25)
	{
		fr->sampling_frequency = 6 + ((newhead >> 10) & 0x3);
	}
	else
		fr->sampling_frequency = ((newhead >> 10) & 0x3) + (fr->lsf * 3);
	fr->error_protection = ((newhead >> 16) & 0x1) ^ 0x1;

	fr->bitrate_index = ((newhead >> 12) & 0xf);
	fr->padding = ((newhead >> 9) & 0x1);
	fr->extension = ((newhead >> 8) & 0x1);
	fr->mode = ((newhead >> 6) & 0x3);
	fr->mode_ext = ((newhead >> 4) & 0x3);
	fr->copyright = ((newhead >> 3) & 0x1);
	fr->original = ((newhead >> 2) & 0x1);
	fr->emphasis = newhead & 0x3;

	fr->stereo = (fr->mode == MPG_MD_MONO) ? 1 : 2;

	ssize = 0;

	if (!fr->bitrate_index)
		return (0);

	switch (fr->lay)
	{
		case 1:
/* 			fr->do_layer = mpg123_do_layer1; */
/* 			mpg123_init_layer2();	/\* inits also shared tables with layer1 *\/ */
			fr->framesize = (long) tabsel_123[fr->lsf][0][fr->bitrate_index] * 12000;
			fr->framesize /= mpg123_freqs[fr->sampling_frequency];
			fr->framesize = ((fr->framesize + fr->padding) << 2) - 4;
			break;
		case 2:
/* 			fr->do_layer = mpg123_do_layer2; */
/* 			mpg123_init_layer2();	/\* inits also shared tables with layer1 *\/ */
			fr->framesize = (long) tabsel_123[fr->lsf][1][fr->bitrate_index] * 144000;
			fr->framesize /= mpg123_freqs[fr->sampling_frequency];
			fr->framesize += fr->padding - 4;
			break;
		case 3:
/* 			fr->do_layer = mpg123_do_layer3; */
			if (fr->lsf)
				ssize = (fr->stereo == 1) ? 9 : 17;
			else
				ssize = (fr->stereo == 1) ? 17 : 32;
			if (fr->error_protection)
				ssize += 2;
			fr->framesize = (long) tabsel_123[fr->lsf][2][fr->bitrate_index] * 144000;
			fr->framesize /= mpg123_freqs[fr->sampling_frequency] << (fr->lsf);
			fr->framesize = fr->framesize + fr->padding - 4;
			break;
		default:
			return (0);
	}
	if(fr->framesize > MAXFRAMESIZE)
		return 0;
	return 1;
}

#define GET_INT32BE(b) \
(i = (b[0] << 24) | (b[1] << 16) | b[2] << 8 | b[3], b += 4, i)

static int mpg123_get_xing_header(xing_header_t * xing, unsigned char *buf)
{	
	int i, head_flags;
	int id, mode;
	
	memset(xing, 0, sizeof(xing_header_t));
	
	/* get selected MPEG header data */ 
	id = (buf[1] >> 3) & 1;
	mode = (buf[3] >> 6) & 3;
	buf += 4;
	
	/* Skip the sub band data */
	if (id)
	{
		/* mpeg1 */
		if (mode != 3)
			buf += 32;
		else
			buf += 17;
	}
	else
	{
		/* mpeg2 */
		if (mode != 3)
			buf += 17;
		else
			buf += 9;
	}
	
	if (strncmp(buf, "Xing", 4))
		return 0;
	buf += 4;
		
	head_flags = GET_INT32BE(buf);
	
	if (head_flags & FRAMES_FLAG)
		xing->frames = GET_INT32BE(buf);
	if (xing->frames < 1)
		xing->frames = 1;
	if (head_flags & BYTES_FLAG)
		xing->bytes = GET_INT32BE(buf);
	
	if (head_flags & TOC_FLAG)
	{
		for (i = 0; i < 100; i++)
			xing->toc[i] = buf[i];
		buf += 100;
	}
	
#ifdef XING_DEBUG
	for (i = 0; i < 100; i++)
	{
		if ((i % 10) == 0)
			fprintf(stderr, "\n");
		fprintf(stderr, " %3d", xing->toc[i]);
	}
#endif
	
	return 1;
}

static double mpg123_compute_tpf(struct frame *fr)
{
	const int bs[4] = {0, 384, 1152, 1152};
	double tpf;

	tpf = bs[fr->lay];
	tpf /= mpg123_freqs[fr->sampling_frequency] << (fr->lsf);
	return tpf;
}

static double mpg123_compute_bpf(struct frame *fr)
{
	double bpf;

	switch (fr->lay)
	{
		case 1:
			bpf = tabsel_123[fr->lsf][0][fr->bitrate_index];
			bpf *= 12000.0 * 4.0;
			bpf /= mpg123_freqs[fr->sampling_frequency] << (fr->lsf);
			break;
		case 2:
		case 3:
			bpf = tabsel_123[fr->lsf][fr->lay - 1][fr->bitrate_index];
			bpf *= 144000;
			bpf /= mpg123_freqs[fr->sampling_frequency] << (fr->lsf);
			break;
		default:
			bpf = 1.0;
	}

	return bpf;
}


unsigned int mpg123_getbits(int number_of_bits)
{
	unsigned long rval;

#ifdef DEBUG_GETBITS
	fprintf(stderr, "g%d", number_of_bits);
#endif

	if(!number_of_bits)
		return 0;

#if 0
	check_buffer_range(number_of_bits + bsi.bitindex);
#endif

	{
		rval = bsi.wordpointer[0];
		rval <<= 8;
		rval |= bsi.wordpointer[1];
		rval <<= 8;
		rval |= bsi.wordpointer[2];

		rval <<= bsi.bitindex;
		rval &= 0xffffff;
		
		bsi.bitindex += number_of_bits;

		rval >>= (24-number_of_bits);

		bsi.wordpointer += (bsi.bitindex >> 3);
		bsi.bitindex &= 7;
	}

#ifdef DEBUG_GETBITS
	fprintf(stderr,":%x ",rval);
#endif

	return rval;
}


void I_step_one(unsigned int balloc[], unsigned int scale_index[2][SBLIMIT], struct frame *fr)
{
	unsigned int *ba = balloc;
	unsigned int *sca = (unsigned int *) scale_index;

	if (fr->stereo)
	{
		int i;
		int jsbound = fr->jsbound;

		for (i = 0; i < jsbound; i++)
		{
			*ba++ = mpg123_getbits(4);
			*ba++ = mpg123_getbits(4);
		}
		for (i = jsbound; i < SBLIMIT; i++)
			*ba++ = mpg123_getbits(4);

		ba = balloc;

		for (i = 0; i < jsbound; i++)
		{
			if ((*ba++))
				*sca++ = mpg123_getbits(6);
			if ((*ba++))
				*sca++ = mpg123_getbits(6);
		}
		for (i = jsbound; i < SBLIMIT; i++)
			if ((*ba++))
			{
				*sca++ = mpg123_getbits(6);
				*sca++ = mpg123_getbits(6);
			}
	}
	else
	{
		int i;

		for (i = 0; i < SBLIMIT; i++)
			*ba++ = mpg123_getbits(4);
		ba = balloc;
		for (i = 0; i < SBLIMIT; i++)
			if ((*ba++))
				*sca++ = mpg123_getbits(6);
	}
}

void I_step_two(real fraction[2][SBLIMIT], unsigned int balloc[2 * SBLIMIT],
		unsigned int scale_index[2][SBLIMIT], struct frame *fr)
{
	int i, n;
	int smpb[2 * SBLIMIT];	/* values: 0-65535 */
	int *sample;
	register unsigned int *ba;
	register unsigned int *sca = (unsigned int *) scale_index;

	if (fr->stereo)
	{
		int jsbound = fr->jsbound;
		register real *f0 = fraction[0];
		register real *f1 = fraction[1];

		ba = balloc;
		for (sample = smpb, i = 0; i < jsbound; i++)
		{
			if ((n = *ba++))
				*sample++ = mpg123_getbits(n + 1);
			if ((n = *ba++))
				*sample++ = mpg123_getbits(n + 1);
		}
		for (i = jsbound; i < SBLIMIT; i++)
			if ((n = *ba++))
				*sample++ = mpg123_getbits(n + 1);

		ba = balloc;
		for (sample = smpb, i = 0; i < jsbound; i++)
		{
			if ((n = *ba++))
				*f0++ = (real) (((-1) << n) + (*sample++) + 1) * mpg123_muls[n + 1][*sca++];
			else
				*f0++ = 0.0;
			if ((n = *ba++))
				*f1++ = (real) (((-1) << n) + (*sample++) + 1) * mpg123_muls[n + 1][*sca++];
			else
				*f1++ = 0.0;
		}
		for (i = jsbound; i < SBLIMIT; i++)
		{
			if ((n = *ba++))
			{
				real samp = (((-1) << n) + (*sample++) + 1);

				*f0++ = samp * mpg123_muls[n + 1][*sca++];
				*f1++ = samp * mpg123_muls[n + 1][*sca++];
			}
			else
				*f0++ = *f1++ = 0.0;
		}
		for (i = fr->down_sample_sblimit; i < 32; i++)
			fraction[0][i] = fraction[1][i] = 0.0;
	}
	else
	{
		register real *f0 = fraction[0];

		ba = balloc;
		for (sample = smpb, i = 0; i < SBLIMIT; i++)
			if ((n = *ba++))
				*sample++ = mpg123_getbits(n + 1);
		ba = balloc;
		for (sample = smpb, i = 0; i < SBLIMIT; i++)
		{
			if ((n = *ba++))
				*f0++ = (real) (((-1) << n) + (*sample++) + 1) * mpg123_muls[n + 1][*sca++];
			else
				*f0++ = 0.0;
		}
		for (i = fr->down_sample_sblimit; i < 32; i++)
			fraction[0][i] = 0.0;
	}
}

static guint get_song_time_file(FILE * file)
{
	guint32 head;
	guchar tmp[4], *buf;
	struct frame frm;
	xing_header_t xing_header;
	double tpf, bpf;
	guint32 len;

	if (!file)
		return -1;

	fseek(file, 0, SEEK_SET);
	if (fread(tmp, 1, 4, file) != 4)
		return 0;
	head = convert_to_header(tmp);
	while (!mpg123_head_check(head))
	{
		head <<= 8;
		if (fread(tmp, 1, 1, file) != 1)
			return 0;
		head |= tmp[0];
	}
	if (mpg123_decode_header(&frm, head))
	{
		buf = g_malloc(frm.framesize + 4);
		fseek(file, -4, SEEK_CUR);
		fread(buf, 1, frm.framesize + 4, file);
		tpf = mpg123_compute_tpf(&frm);
		if (mpg123_get_xing_header(&xing_header, buf))
		{
			g_free(buf);
			return ((guint) (tpf * xing_header.frames * 1000));
		}
		g_free(buf);
		bpf = mpg123_compute_bpf(&frm);
		fseek(file, 0, SEEK_END);
		len = ftell(file);
		fseek(file, -128, SEEK_END);
		fread(tmp, 1, 3, file);
		if (!strncmp(tmp, "TAG", 3))
			len -= 128;
		return ((guint) ((guint)(len / bpf) * tpf * 1000));
	}
	return 0;
}

guint get_song_time (gchar *path)
{
    FILE *file = fopen (path, "r");
    guint result = get_song_time_file (file);
    fclose (file);
    return result;
}
