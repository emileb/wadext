
#include <jni.h>

#include <android/log.h>
#include <string.h>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include "wadext.h"
#include "resourcefile.h"
#include "fileformat.h"
#include "wadman.h"
#include <sys/stat.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO,"wadext", __VA_ARGS__))



#define JAVA_FUNC(x) Java_com_opentouchgaming_androidcore_WadExt_##x

#define EXPORT_ME __attribute__ ((visibility("default")))

static bool pstartfound=false;
static WadItemList PNames(-1);

static char* strupr(char* str)
{
	for (char* ch = str; '\0' != *ch; ++ch)
	{
		*ch = toupper(*ch);
	}

	return str;
}

static void OpenWad(const char *filename)
{
	mainwad = new CWADFile(filename);
	if (mainwad->NumLumps() == 0)
	{
		//exit(1);
	}
}

static bool isPatch(const char * n)
{
	if (!pstartfound || PNames.mLump < 0) return false;
	uint32_t * l = (uint32_t*)PNames.Address();
	char * c = (char*)(l + 1);
	char nn[9];

	if (!l) return false;

	strncpy(nn, n, 8);
	nn[8] = 0;
	strupr(nn);

	for (int i = 0; i<*l; i++)
	{
		if (!strncmp(c, nn, 8)) return true;
		c += 8;
	}
	return false;
}

//void Extract(WadItemList *w,const char * dir,const char * ext, int options, bool isflat);	//It is completely impossible to detect flats, so we have to flag it when extracting the F_ namespace
static void Extract(WadItemList *w, int options, bool isflat, const char *filename)	//It is completely impossible to detect flats, so we have to flag it when extracting the F_ namespace
{
	FileType ft = IdentifyFileType(w->Name(), (uint8_t*)w->Address(), w->Length());

	// Flats are easy to misidentify so if we are in the flat namespace and the size is exactly 4096 bytes, let it pass as flat
	if (isflat && (ft.type & FG_MASK) != FG_GFX && (ft.type == FT_DOOMSND || ft.type == FT_MP3 || w->Length() == 4096))
	{
		ft.type = FT_BINARY;
		ft.extension = ".LMP";
	}

	printf("processing %.8s\n", w->Name());

	if (ft.type == FT_DOOMGFX && !(options & NO_CONVERT_GFX))
	{
		PatchToPng(options, (const uint8_t*)w->Address(), w->Length(), filename);
	}
	else if (w->Length() > 0 && ft.type == FT_BINARY && !(options & NO_CONVERT_GFX) && (isflat || (w->Length() == 64000 && options&(DO_HEXEN_PAL | DO_HERETIC_PAL))))
	{
		FlatToPng(options, (const uint8_t*)w->Address(), w->Length(), filename);
	}
	else if (ft.type == FT_DOOMSND && !(options & NO_CONVERT_SND))
	{
		DoomSndToWav((const uint8_t*)w->Address(), w->Length(), filename);
	}
	else
	{
		FILE *f = fopen(filename, "wb");
		if (f == NULL)
		{
			printf("%s: Unable to create file\n", filename);
			return;
		}
		fwrite(w->Address(), 1, w->Length(), f);
		fclose(f);
	}
}

extern "C"
{
    jint EXPORT_ME  JAVA_FUNC(extract) ( JNIEnv* env, jobject thiz, jstring wadFile, jstring lumpName, jint options, jint isFlat, jstring outFile )
    {
       // chdir("/sdcard/wad");

    	const char *wadFile_s = (char *)(env)->GetStringUTFChars( wadFile, 0);
        const char *lumpName_s = (char *)(env)->GetStringUTFChars( lumpName, 0);
        const char *outFile_s = (char *)(env)->GetStringUTFChars( outFile, 0);

        LOGI("Wad file: %s, Looking for %s, Output %s", wadFile_s, lumpName_s, outFile_s);

        OpenWad(wadFile_s);

        int nbrLumps = mainwad->NumLumps();
        if (nbrLumps == 0)
        {
            LOGI("No lumps found");
        }
        else
        {
            LOGI("Number of lumps: %d", nbrLumps);
        }

        for( int n = 0; n < nbrLumps; n++ )
        {
            //LOGI("Lump %s", wad->GetLumpName(n) );
        }

        int titlepic = mainwad->FindLump(lumpName_s);

        LOGI("tp = %d", titlepic);

        if( titlepic != -1 )
        {
            WadItemList ww(titlepic);
            WadItemList *w = &ww;

            Extract(w, options, isFlat, outFile_s);
        }

        env->ReleaseStringUTFChars( wadFile, wadFile_s);
        env->ReleaseStringUTFChars( lumpName, lumpName_s);
        env->ReleaseStringUTFChars( outFile, outFile_s);

        delete mainwad;
        return 0;
    }



}