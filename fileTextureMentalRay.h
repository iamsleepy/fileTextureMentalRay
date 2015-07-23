// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the FILETEXTUREMENTALRAY_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// FILETEXTUREMENTALRAY_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef FILETEXTUREMENTALRAY_EXPORTS
#define FILETEXTUREMENTALRAY_API __declspec(dllexport)
#else
#define FILETEXTUREMENTALRAY_API __declspec(dllimport)
#endif

// This class is exported from the fileTextureMentalRay.dll
class FILETEXTUREMENTALRAY_API CfileTextureMentalRay {
public:
	CfileTextureMentalRay(void);
	// TODO: add your methods here.
};

extern FILETEXTUREMENTALRAY_API int nfileTextureMentalRay;

FILETEXTUREMENTALRAY_API int fnfileTextureMentalRay(void);
