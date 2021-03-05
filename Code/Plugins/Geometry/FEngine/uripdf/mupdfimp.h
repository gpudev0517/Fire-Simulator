#ifndef MUPDF_IMPORT_H
#define MUPDF_IMPORT_H

#ifdef _WIN32
#include <windows.h>
#define TCHAR wchar_t
#else
#define MAX_PATH    1024
#define TCHAR char
#endif

extern "C" {
#ifdef _WIN32
#ifdef MUPDF_IMP
	#define MUPDF_API extern __declspec(dllexport)
#else
	#define MUPDF_API extern __declspec(dllimport)
#endif  /* MUPDF_IMP_DLL */
#else
	#define MUPDF_API
#endif

MUPDF_API void*  pdf_app_init(int w, int h);
MUPDF_API int pdf_app_finalize(void* vParam);
MUPDF_API int pdf_app_loadfile(void* vParam, TCHAR* wfilename, const char* password);
MUPDF_API int pdf_app_get_outline_count(void* vParam);
MUPDF_API void* pdf_app_get_outlines(void* vParam);
MUPDF_API void* pdf_app_search_text(void* vParam, TCHAR* wSearch, int* hitCount);
MUPDF_API void pdf_app_get_pagetext(void* vParam, wchar_t* pageText, int size);
MUPDF_API int pdf_app_close(void* vParam);
MUPDF_API int pdf_app_resize(void* vParam, int w, int h); // auto-fit
MUPDF_API int pdf_app_horizontal_fit(void* vParam, int w, int h);
MUPDF_API int pdf_app_vertical_fit(void* vParam, int w, int h);
MUPDF_API int pdf_app_zoomin(void* vParam);
MUPDF_API int pdf_app_zoomout(void* vParam);
MUPDF_API int pdf_app_view_page(void* vParam, int n);
MUPDF_API int pdf_app_view_page_next(void* vParam);
MUPDF_API int pdf_app_view_page_prev(void* vParam);
MUPDF_API unsigned char* pdf_app_image(void* vParam, int *w, int *h, int* n);
MUPDF_API unsigned char* pdf_app_image_with_number(void* vParam, int page, int *w, int *h, int* n);
MUPDF_API unsigned char* pdf_app_image_with_number_and_zoom(void* vParam, int page, int *w, int *h, int* n, int zoom);
MUPDF_API unsigned char* pdf_app_image_with_number_without_zoom(void* vParam, int page, int *w, int *h, int* n);
MUPDF_API void pdf_app_origin_size(void* vParam, int page, int *w, int *h);
MUPDF_API int pdf_app_save_thumb(void* vParam, int page, TCHAR* wthumbfilename);
#ifdef _WIN32
MUPDF_API int pdf_app_drawpage(void* vParam, HDC hDC, int page, int x, int y, int *w, int *h, int* n);
#endif
MUPDF_API int pdf_app_pagecount(void* vParam);
MUPDF_API char* pdf_app_title(void* vParam);
MUPDF_API TCHAR* pdf_app_title2(void* vParam);
MUPDF_API int pdf_app_author(void* vParam);
MUPDF_API int pdf_app_current_zoom_scale(void* vParam);

typedef struct tagOutLineItem
{
	int page;
        TCHAR title[MAX_PATH]; //unicode
	int level;
} OutLineItem;

typedef struct tagRectF
{
	float x1, y1;
	float x2, y2;
} RectF;

}

#endif
