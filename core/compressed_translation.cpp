/*************************************************************************/
/*  compressed_translation.cpp                                           */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                    http://www.godotengine.org                         */
/*************************************************************************/
/* Copyright (c) 2007-2015 Juan Linietsky, Ariel Manzur.                 */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/
#include "compressed_translation.h"
#include "pair.h"
#include <string.h>

/////////// SMAZ /////////////

/*
Copyright (c) 2006-2009, Salvatore Sanfilippo
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of Smaz nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


/* Our compression codebook, used for compression */
static const char *Smaz_cb[241] = {
"\002s,\266", "\003had\232\002leW", "\003on \216", "", "\001yS",
"\002ma\255\002li\227", "\003or \260", "", "\002ll\230\003s t\277",
"\004fromg\002mel", "", "\003its\332", "\001z\333", "\003ingF", "\001>\336",
"\001 \000\003 (\002nc\344", "\002nd=\003 on\312",
"\002ne\213\003hat\276\003re q", "", "\002ngT\003herz\004have\306\003s o\225",
"", "\003ionk\003s a\254\002ly\352", "\003hisL\003 inN\003 be\252", "",
"\003 fo\325\003 of \003 ha\311", "", "\002of\005",
"\003 co\241\002no\267\003 ma\370", "", "", "\003 cl\356\003enta\003 an7",
"\002ns\300\001\"e", "\003n t\217\002ntP\003s, \205",
"\002pe\320\003 we\351\002om\223", "\002on\037", "", "\002y G", "\003 wa\271",
"\003 re\321\002or*", "", "\002=\"\251\002ot\337", "\003forD\002ou[",
"\003 toR", "\003 th\r", "\003 it\366",
"\003but\261\002ra\202\003 wi\363\002</\361", "\003 wh\237", "\002 4",
"\003nd ?", "\002re!", "", "\003ng c", "",
"\003ly \307\003ass\323\001a\004\002rir", "", "", "", "\002se_", "\003of \"",
"\003div\364\002ros\003ere\240", "", "\002ta\310\001bZ\002si\324", "",
"\003and\a\002rs\335", "\002rt\362", "\002teE", "\003ati\316", "\002so\263",
"\002th\021", "\002tiJ\001c\034\003allp", "\003ate\345", "\002ss\246",
"\002stM", "", "\002><\346", "\002to\024", "\003arew", "\001d\030",
"\002tr\303", "", "\001\n1\003 a \222", "\003f tv\002veo", "\002un\340", "",
"\003e o\242", "\002a \243\002wa\326\001e\002", "\002ur\226\003e a\274",
"\002us\244\003\n\r\n\247", "\002ut\304\003e c\373", "\002we\221", "", "",
"\002wh\302", "\001f,", "", "", "", "\003d t\206", "", "", "\003th \343",
"\001g;", "", "", "\001\r9\003e s\265", "\003e t\234", "", "\003to Y",
"\003e\r\n\236", "\002d \036\001h\022", "", "\001,Q", "\002 a\031", "\002 b^",
"\002\r\n\025\002 cI", "\002 d\245", "\002 e\253", "\002 fh\001i\b\002e \v",
"", "\002 hU\001-\314", "\002 i8", "", "", "\002 l\315", "\002 m{",
"\002f :\002 n\354", "\002 o\035", "\002 p}\001.n\003\r\n\r\250", "",
"\002 r\275", "\002 s>", "\002 t\016", "", "\002g \235\005which+\003whi\367",
"\002 w5", "\001/\305", "\003as \214", "\003at \207", "", "\003who\331", "",
"\001l\026\002h \212", "", "\002, $", "", "\004withV", "", "", "", "\001m-", "",
"", "\002ac\357", "\002ad\350", "\003TheH", "", "", "\004this\233\001n\t",
"", "\002. y", "", "\002alX\003e, \365", "\003tio\215\002be\\",
"\002an\032\003ver\347", "", "\004that0\003tha\313\001o\006", "\003was2",
"\002arO", "\002as.", "\002at'\003the\001\004they\200\005there\322\005theird",
"\002ce\210", "\004were]", "", "\002ch\231\002l \264\001p<", "", "",
"\003one\256", "", "\003he \023\002dej", "\003ter\270", "\002cou", "",
"\002by\177\002di\201\002eax", "", "\002ec\327", "\002edB", "\002ee\353", "",
"", "\001r\f\002n )", "", "", "", "\002el\262", "", "\003in i\002en3", "",
"\002o `\001s\n", "", "\002er\033", "\003is t\002es6", "", "\002ge\371",
"\004.com\375", "\002fo\334\003our\330", "\003ch \301\001t\003", "\002hab", "",
"\003men\374", "", "\002he\020", "", "", "\001u&", "\002hif", "",
"\003not\204\002ic\203", "\003ed @\002id\355", "", "", "\002ho\273",
"\002r K\001vm", "", "", "", "\003t t\257\002il\360", "\002im\342",
"\003en \317\002in\017", "\002io\220", "\002s \027\001wA", "", "\003er |",
"\003es ~\002is%", "\002it/", "", "\002iv\272", "",
"\002t #\ahttp://C\001x\372", "\002la\211", "\001<\341", "\003, a\224"
};

/* Reverse compression codebook, used for decompression */
static const char *Smaz_rcb[254] = {
" ", "the", "e", "t", "a", "of", "o", "and", "i", "n", "s", "e ", "r", " th",
" t", "in", "he", "th", "h", "he ", "to", "\r\n", "l", "s ", "d", " a", "an",
"er", "c", " o", "d ", "on", " of", "re", "of ", "t ", ", ", "is", "u", "at",
" ", "n ", "or", "which", "f", "m", "as", "it", "that", "\n", "was", "en",
" ", " w", "es", " an", " i", "\r", "f ", "g", "p", "nd", " s", "nd ", "ed ",
"w", "ed", "http://", "for", "te", "ing", "y ", "The", " c", "ti", "r ", "his",
"st", " in", "ar", "nt", ",", " to", "y", "ng", " h", "with", "le", "al", "to ",
"b", "ou", "be", "were", " b", "se", "o ", "ent", "ha", "ng ", "their", "\"",
"hi", "from", " f", "in ", "de", "ion", "me", "v", ".", "ve", "all", "re ",
"ri", "ro", "is ", "co", "f t", "are", "ea", ". ", "her", " m", "er ", " p",
"es ", "by", "they", "di", "ra", "ic", "not", "s, ", "d t", "at ", "ce", "la",
"h ", "ne", "as ", "tio", "on ", "n t", "io", "we", " a ", "om", ", a", "s o",
"ur", "li", "ll", "ch", "had", "this", "e t", "g ", "e\r\n", " wh", "ere",
" co", "e o", "a ", "us", " d", "ss", "\n\r\n", "\r\n\r", "=\"", " be", " e",
"s a", "ma", "one", "t t", "or ", "but", "el", "so", "l ", "e s", "s,", "no",
"ter", " wa", "iv", "ho", "e a", " r", "hat", "s t", "ns", "ch ", "wh", "tr",
"ut", "/", "have", "ly ", "ta", " ha", " on", "tha", "-", " l", "ati", "en ",
"pe", " re", "there", "ass", "si", " fo", "wa", "ec", "our", "who", "its", "z",
"fo", "rs", ">", "ot", "un", "<", "im", "th ", "nc", "ate", "><", "ver", "ad",
" we", "ly", "ee", " n", "id", " cl", "ac", "il", "</", "rt", " wi", "div",
"e, ", " it", "whi", " ma", "ge", "x", "e c", "men", ".com"
};

static int smaz_compress(const char *in, int inlen, char *out, int outlen) {
    unsigned int h1,h2,h3=0;
    int verblen = 0, _outlen = outlen;
    char verb[256], *_out = out;

    while(inlen) {
	int j = 7, needed;
	char *flush = NULL;
	const char *slot;

	h1 = h2 = in[0]<<3;
	if (inlen > 1) h2 += in[1];
	if (inlen > 2) h3 = h2^in[2];
	if (j > inlen) j = inlen;

	/* Try to lookup substrings into the hash table, starting from the
* longer to the shorter substrings */
	for (; j > 0; j--) {
	    switch(j) {
	    case 1: slot = Smaz_cb[h1%241]; break;
	    case 2: slot = Smaz_cb[h2%241]; break;
	    default: slot = Smaz_cb[h3%241]; break;
	    }
	    while(slot[0]) {
		if (slot[0] == j && memcmp(slot+1,in,j) == 0) {
		    /* Match found in the hash table,
* prepare a verbatim bytes flush if needed */
		    if (verblen) {
			needed = (verblen == 1) ? 2 : 2+verblen;
			flush = out;
			out += needed;
			outlen -= needed;
		    }
		    /* Emit the byte */
		    if (outlen <= 0) return _outlen+1;
		    out[0] = slot[slot[0]+1];
		    out++;
		    outlen--;
		    inlen -= j;
		    in += j;
		    goto out;
		} else {
		    slot += slot[0]+2;
		}
	    }
	}
	/* Match not found - add the byte to the verbatim buffer */
	verb[verblen] = in[0];
	verblen++;
	inlen--;
	in++;
out:
	/* Prepare a flush if we reached the flush length limit, and there
* is not already a pending flush operation. */
	if (!flush && (verblen == 256 || (verblen > 0 && inlen == 0))) {
	    needed = (verblen == 1) ? 2 : 2+verblen;
	    flush = out;
	    out += needed;
	    outlen -= needed;
	    if (outlen < 0) return _outlen+1;
	}
	/* Perform a verbatim flush if needed */
	if (flush) {
	    if (verblen == 1) {
		flush[0] = (signed char)254;
		flush[1] = verb[0];
	    } else {
		flush[0] = (signed char)255;
		flush[1] = (signed char)(verblen-1);
		memcpy(flush+2,verb,verblen);
	    }
	    flush = NULL;
	    verblen = 0;
	}
    }
    return out-_out;
}

static int smaz_decompress(const char *in, int inlen, char *out, int outlen) {
    unsigned char *c = (unsigned char*) in;
    char *_out = out;
    int _outlen = outlen;

    while(inlen) {
	if (*c == 254) {
	    /* Verbatim byte */
	    if (outlen < 1) return _outlen+1;
	    *out = *(c+1);
	    out++;
	    outlen--;
	    c += 2;
	    inlen -= 2;
	} else if (*c == 255) {
	    /* Verbatim string */
	    int len = (*(c+1))+1;
	    if (outlen < len) return _outlen+1;
	    memcpy(out,c+2,len);
	    out += len;
	    outlen -= len;
	    c += 2+len;
	    inlen -= 2+len;
	} else {
	    /* Codebook entry */
	    const char *s = Smaz_rcb[*c];
	    int len = strlen(s);

	    if (outlen < len) return _outlen+1;
	    memcpy(out,s,len);
	    out += len;
	    outlen -= len;
	    c++;
	    inlen--;
	}
    }
    return out-_out;
}


/////////// END OF SMAZ /////////////

struct _PHashTranslationCmp {

	int orig_len;
	CharString compressed;
	int offset;
};

void PHashTranslation::generate(const Ref<Translation> &p_from) {
#ifdef TOOLS_ENABLED
	List<StringName> keys;
	p_from->get_message_list(&keys);

	int size=Math::larger_prime(keys.size());


	print_line("compressing keys: "+itos(keys.size()));
	Vector< Vector< Pair<int,CharString> > > buckets;
	Vector< Map< uint32_t, int > > table;
	Vector< uint32_t > hfunc_table;
	Vector< _PHashTranslationCmp > compressed;

	table.resize(size);
	hfunc_table.resize(size);
	buckets.resize(size);
	compressed.resize(keys.size());

	int idx=0;
	int total_compression_size=0;
	int total_string_size=0;

	for(List<StringName>::Element *E=keys.front();E;E=E->next()) {

		//hash string
		CharString cs = E->get().operator String().utf8();
		uint32_t h = hash(0,cs.get_data());
		Pair<int,CharString> p;
		p.first=idx;
		p.second=cs;
		buckets[h % size].push_back(p);

		//compress string
		CharString src_s = p_from->get_message(E->get()).operator String().utf8();
		_PHashTranslationCmp ps;
		ps.orig_len=src_s.size();
		ps.offset=total_compression_size;

		if (ps.orig_len!=0) {
			CharString dst_s;
			dst_s.resize(src_s.size());
			int ret = smaz_compress(src_s.get_data(),src_s.size(),&dst_s[0],src_s.size());
			if (ret>=src_s.size()) {
				//if compressed is larger than original, just use original
				ps.orig_len=src_s.size();
				ps.compressed=src_s;
			} else {
				dst_s.resize(ret);
				//ps.orig_len=;
				ps.compressed=dst_s;
			}
		} else {
			ps.orig_len=1;
			ps.compressed.resize(1);
			ps.compressed[0]=0;
		}


		compressed[idx]=ps;
		total_compression_size+=ps.compressed.size();
		total_string_size+=src_s.size();
		idx++;
	}

	int bucket_table_size=0;
	print_line("total compressed string size: "+itos(total_compression_size)+" ("+itos(total_string_size)+" uncompressed).");

	for(int i=0;i<size;i++) {

		Vector< Pair<int,CharString> > &b = buckets[i];
		Map< uint32_t, int > &t=table[i];

		if (b.size()==0)
			continue;

		//print_line("bucket: "+itos(i)+" - elements: "+itos(b.size()));

		int d = 1;
		int item =0;

		while(item < b.size()) {

			uint32_t slot = hash(d,b[item].second.get_data());
			if (t.has(slot)) {

				item=0;
				d++;
				t.clear();
			} else {
				t[slot]=b[item].first;
				item++;
			}
		}

		hfunc_table[i]=d;
		bucket_table_size+=2+b.size()*4;

	}


	print_line("bucket table size: "+itos(bucket_table_size*4));
	print_line("hash table size: "+itos(size*4));

	hash_table.resize(size);
	bucket_table.resize(bucket_table_size);

	DVector<int>::Write htwb = hash_table.write();
	DVector<int>::Write btwb = bucket_table.write();

	uint32_t *htw = (uint32_t*)&htwb[0];
	uint32_t *btw = (uint32_t*)&btwb[0];

	int btindex=0;
	int collisions=0;

	for(int i=0;i<size;i++) {

		Map< uint32_t, int > &t=table[i];
		if (t.size()==0) {
			htw[i]=0xFFFFFFFF; //nothing
			continue;
		} else if (t.size()>1) {
			collisions+=t.size()-1;
		}

		htw[i]=btindex;
		btw[btindex++]=t.size();
		btw[btindex++]=hfunc_table[i];

		for( Map< uint32_t, int >::Element *E=t.front();E;E=E->next()) {

			btw[btindex++]=E->key();
			btw[btindex++]=compressed[E->get()].offset;
			btw[btindex++]=compressed[E->get()].compressed.size();
			btw[btindex++]=compressed[E->get()].orig_len;
		}

	}

	print_line("total collisions: "+itos(collisions));

	strings.resize(total_compression_size);
	DVector<uint8_t>::Write cw = strings.write();

	for(int i=0;i<compressed.size();i++) {
		memcpy(&cw[compressed[i].offset],compressed[i].compressed.get_data(),compressed[i].compressed.size());
	}


	ERR_FAIL_COND(btindex!=bucket_table_size);
	set_locale(p_from->get_locale());

#endif
}

bool PHashTranslation::_set(const StringName& p_name, const Variant& p_value) {

	String name = p_name.operator String();
	if (name=="hash_table") {
		hash_table=p_value;
		print_line("translation: loaded hash table of size: "+itos(hash_table.size()));
	} else if (name=="bucket_table") {
		bucket_table=p_value;
		print_line("translation: loaded bucket table of size: "+itos(bucket_table.size()));
	} else if (name=="strings") {
		strings=p_value;
		print_line("translation: loaded string table of size: "+itos(strings.size()));
	} else if (name=="load_from") {
		print_line("generating");
		generate(p_value);
	} else
		return false;

	return true;

}

bool PHashTranslation::_get(const StringName& p_name,Variant &r_ret) const{

	String name = p_name.operator String();
	if (name=="hash_table")
		r_ret=hash_table;
	else if (name=="bucket_table")
		r_ret=bucket_table;
	else if (name=="strings")
		r_ret=strings;
	else
		return false;

	return true;

}

StringName PHashTranslation::get_message(const StringName& p_src_text) const {

	int htsize = hash_table.size();

	if (htsize==0)
		return StringName();

	CharString str = p_src_text.operator String().utf8();
	uint32_t h = hash(0,str.get_data());


	DVector<int>::Read htr =  hash_table.read();
	const uint32_t *htptr = (const uint32_t*)&htr[0];
	DVector<int>::Read btr =  bucket_table.read();
	const uint32_t *btptr = (const uint32_t*)&btr[0];
	DVector<uint8_t>::Read sr = strings.read();
	const char *sptr= (const char*)&sr[0];

	uint32_t p = htptr[ h % htsize];

	//print_line("String: "+p_src_text.operator String());
	//print_line("Hash: "+itos(p));

	if (p==0xFFFFFFFF) {
//		print_line("GETMSG: Nothing!");
		return StringName(); //nothing
	}

	const Bucket &bucket = *(const Bucket*)&btptr[p];

	h = hash(bucket.func,str.get_data());

	int idx=-1;

	for(int i=0;i<bucket.size;i++) {

		if (bucket.elem[i].key==h) {

			idx=i;
			break;
		}

	}

	//print_line("bucket pos: "+itos(idx));
	if (idx==-1) {
//		print_line("GETMSG: Not in Bucket!");
		return StringName();
	}

	if (bucket.elem[idx].comp_size == bucket.elem[idx].uncomp_size) {

		String rstr;
		rstr.parse_utf8(&sptr[ bucket.elem[idx].str_offset ], bucket.elem[idx].uncomp_size );
//		print_line("Uncompressed, size: "+itos(bucket.elem[idx].comp_size));
//		print_line("Return: "+rstr);

		return rstr;
	} else {

		CharString uncomp;
		uncomp.resize( bucket.elem[idx].uncomp_size+1 );
		smaz_decompress(&sptr[ bucket.elem[idx].str_offset ], bucket.elem[idx].comp_size,uncomp.ptr(),bucket.elem[idx].uncomp_size );
		String rstr;
		rstr.parse_utf8(uncomp.get_data());
//		print_line("Compressed, size: "+itos(bucket.elem[idx].comp_size));
//		print_line("Return: "+rstr);
		return rstr;
	}

}


void PHashTranslation::_get_property_list( List<PropertyInfo> *p_list) const{

	p_list->push_back( PropertyInfo(Variant::INT_ARRAY, "hash_table"));
	p_list->push_back( PropertyInfo(Variant::INT_ARRAY, "bucket_table"));
	p_list->push_back( PropertyInfo(Variant::RAW_ARRAY, "strings"));
	p_list->push_back( PropertyInfo(Variant::OBJECT, "load_from",PROPERTY_HINT_RESOURCE_TYPE,"Translation",PROPERTY_USAGE_EDITOR));

}
void PHashTranslation::_bind_methods() {

	ObjectTypeDB::bind_method(_MD("generate","from:Translation"),&PHashTranslation::generate);
}

PHashTranslation::PHashTranslation()
{
}
