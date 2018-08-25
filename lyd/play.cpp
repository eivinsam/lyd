#define NOMINMAX
#include <Windows.h>
#include <mmreg.h>

void terminate(int x) { ExitProcess(x); }

void* alloc(int bytes)
{
	return GlobalAlloc(GMEM_FIXED, bytes);
}

void play(float* buffer, int length, int sample_freq)
{
	HWAVEOUT handle;

	{
		WAVEFORMATEX format;
		format.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
		format.nChannels = 1;
		format.nSamplesPerSec = sample_freq;
		format.wBitsPerSample = sizeof(float) * 8;
		format.nBlockAlign = format.nChannels * sizeof(float);
		format.nAvgBytesPerSec = sample_freq * sizeof(float);
		format.cbSize = 0;

		if (auto res = waveOutOpen(&handle, WAVE_MAPPER, &format, NULL, NULL, CALLBACK_NULL);
			res != MMSYSERR_NOERROR)
			return;
	}
	WAVEHDR header;
	header.lpData = (char*)buffer;
	header.dwBufferLength = length * sizeof(float);
	header.dwFlags = 0;

	if (MMSYSERR_NOERROR != waveOutPrepareHeader(handle, &header, sizeof(header)))
		return;

	waveOutPause(handle);

	if (MMSYSERR_NOERROR != waveOutWrite(handle, &header, sizeof(header)))
		return;

	Sleep(500);
	waveOutRestart(handle);

	Sleep(length * 1000 / sample_freq);
}
