#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>

#define blue 0x1F
#define red 0x7C
#define green 0xE0
#define alpha 0x03

LRESULT CALLBACK EventProcessor(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return(DefWindowProc(hWnd, uMsg, wParam, lParam));
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine, int nCmdShow)
{
	WNDCLASS		wc;
	HWND			WindowHandle;
	int			ROWS, COLS, i, BYTES;
	unsigned char	*displaydata;
	BITMAPINFO		*bm_info;
	HDC			hDC;

	// Text was display garbage so i had to take this approch to see english text
	TCHAR mbox1[] = _T("Image Window CLass");
	TCHAR mbox2[] = _T("ECE468 Lab1");
	TCHAR mbox3[] = _T("No window");
	TCHAR mbox4[] = _T("Try again");
	TCHAR mbox5[] = _T("Press OK to continue");
	TCHAR mbox6[] = _T(" ");

	FILE *fpt, *fpt2;
	char Header[80];
	unsigned char *input_image;

	if ((fpt = fopen(lpCmdLine, "rb")) == NULL) {

		MessageBox(NULL, mbox4, mbox5, MB_OK | MB_APPLMODAL);
		exit(0);
	}

	fscanf(fpt, "%s %d %d %d", Header, &COLS, &ROWS, &BYTES);
	Header[79] = fgetc(fpt);
	displaydata = (unsigned char *)calloc((ROWS*COLS * 3), 1);

	if (Header[1] == '6') {

		input_image = (unsigned char *)calloc((ROWS*COLS * 3), 1);
		fread(input_image, 1, (ROWS*COLS * 3), fpt);

		for (i = 0; i < (ROWS*COLS); i++) {

			displaydata[(i * 2) + 0] = input_image[(i * 3) + 2] >> 3;
			displaydata[(i * 2) + 1] = input_image[(i * 3) + 0] >> 1;
			displaydata[(i * 2) + 1] = displaydata[(i * 2) + 1] & red;
			displaydata[(i * 2) + 0] = displaydata[(i * 2) + 0] & blue;
			displaydata[(i * 2) + 1] = displaydata[(i * 2) + 1] | ((input_image[(i * 3) + 1] >> 6) & alpha);
			displaydata[(i * 2) + 0] = displaydata[(i * 2) + 0] | ((input_image[(i * 3) + 1] << 2) & green);

		}

		fpt2 = fopen("test_image1.ppm", "wb");
		fprintf(fpt2, "P6 %d %d 255\n", COLS, ROWS);
		fwrite(displaydata, 1, ROWS*COLS * 3, fpt2);
		fclose(fpt2);
	}

	else {

		input_image = (unsigned char *)calloc((ROWS*COLS), 1);
		fread(input_image, 1, (ROWS*COLS), fpt);

		for (i = 0; i < (ROWS*COLS); i++) {

			displaydata[i] = input_image[i];

		}

		fpt2 = fopen("test_image2.ppm", "wb");
		fprintf(fpt2, "P5 %d %d 255\n", COLS, ROWS);
		fwrite(displaydata, 1, ROWS*COLS, fpt2);
		fclose(fpt2);
	}

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)EventProcessor;
	wc.cbClsExtra = wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = wc.lpszMenuName = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszClassName = mbox1;
	if (RegisterClass(&wc) == 0)
		exit(0);

	WindowHandle = CreateWindow(mbox1, mbox2,
		WS_OVERLAPPEDWINDOW,
		10, 10, COLS, ROWS,
		NULL, NULL, hInstance, NULL);
	if (WindowHandle == NULL)
	{
		MessageBox(NULL, mbox3, mbox4, MB_OK | MB_APPLMODAL);
		exit(0);
	}
	ShowWindow(WindowHandle, SW_SHOWNORMAL);

	bm_info = (BITMAPINFO *)calloc(1, sizeof(BITMAPINFO) + 256 * sizeof(RGBQUAD));
	hDC = GetDC(WindowHandle);

	/* ... set up bmiHeader field of bm_info ... */
	bm_info->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

	if (Header[1] == '6') {
		bm_info->bmiHeader.biBitCount = 16;
	}

	else {
		bm_info->bmiHeader.biBitCount = 8;
	}

	bm_info->bmiHeader.biSizeImage = (ROWS*COLS * 3);
	bm_info->bmiHeader.biHeight = -ROWS;
	bm_info->bmiHeader.biWidth = COLS;
	bm_info->bmiHeader.biPlanes = 1;
	bm_info->bmiHeader.biCompression = BI_RGB;


	for (i = 0; i<256; i++)	/* colormap */
	{
		bm_info->bmiColors[i].rgbBlue = bm_info->bmiColors[i].rgbGreen = bm_info->bmiColors[i].rgbRed = i;
		bm_info->bmiColors[i].rgbReserved = 0;
	}


	if (Header[1] == '6') {

		SetDIBitsToDevice(hDC, 0, 0, COLS, ROWS + 80, 0, 0,
			0, /* first scan line */
			COLS, /* number of scan lines */
			displaydata, bm_info, DIB_RGB_COLORS);
	}

	else {

		SetDIBitsToDevice(hDC, 0, 0, COLS, ROWS, 0, 0,
			0, /* first scan line */
			COLS, /* number of scan lines */
			displaydata, bm_info, DIB_RGB_COLORS);
	}

	ReleaseDC(WindowHandle, hDC);
	free(bm_info);
	MessageBox(NULL, mbox5, mbox6, MB_OK | MB_APPLMODAL);
	free(displaydata);
	free(input_image);
	fclose(fpt);
	exit(0);
}