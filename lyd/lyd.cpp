#include <type_traits>

#include "fixed.h"

constexpr int sample_freq = 48000;
constexpr int beat_len = (48000 * 60) / (120 * 4);
//constexpr float bpm = sample_frequency / 4.0f / step_length;
constexpr int pat_len = 16;

enum class note : char { none };

inline bool is_note(note n) { return n != note::none; }

constexpr note operator+(note a, char b) { return note(char(a) + b); }

namespace melody 
{
	constexpr note __ = note::none;

	constexpr note key = __ + 48;
	constexpr note S0 = key + 0, s0 = S0 + (-12);
	constexpr note S1 = key + 2, s1 = S1 + (-12);
	constexpr note S2 = key + 4, s2 = S2 + (-12);
	constexpr note S3 = key + 5, s3 = S3 + (-12);
	constexpr note S4 = key + 7, s4 = S4 + (-12);
	constexpr note S5 = key + 9, s5 = S5 + (-12);


	static constexpr note patterns[][pat_len] =
	{
	{ __,__,__,__, __,__,__,__, __,__,__,__, __,__,__,__ },
	{ s0,__,__,__, __,__,__,__, __,__,s0,__, __,__,__,__ },
	{ S0,__,__,__, S1,__,__,__, S2,__,__,__, S3,__,__,__ },
	{ S4,__,__,__, __,__,__,__, S4,__,__,__, __,__,__,__ },
	{ S5,__,__,__, S5,__,__,__, S5,__,__,__, S5,__,__,__ },
	{ S4,__,__,__, __,__,__,__, __,__,__,__, __,__,__,__ },
	{ S3,__,__,__, S3,__,__,__, S3,__,__,__, S3,__,__,__ },
	{ S2,__,__,__, __,__,__,__, S2,__,__,__, __,__,__,__ },
	{ S1,__,__,__, S1,__,__,__, S1,__,__,__, S1,__,__,__ },
	{ S0,__,__,__, __,__,__,__, __,__,__,__, __,__,__,__ },
	{ s0,__,__,s0, __,__,s0,__, __,s0,__,__, s0,__,s0,__ },
	{ s4,__,__,s4, __,__,s4,__, __,s4,__,__, s4,__,s4,__ },
	{ s5,__,__,s5, __,__,s5,__, __,s5,__,__, s5,__,s5,__ },
	{ s4,__,__,__, __,__,s4,__, __,__,__,__, s4,__,__,__ },
	{ s3,__,__,s3, __,__,s3,__, __,s3,__,__, s3,__,s3,__ },
	{ s2,__,__,s2, __,__,s2,__, __,s2,__,__, s2,__,s2,__ },
	{ s1,__,__,s1, __,__,s1,__, __,s1,__,__, s1,__,s1,s1 },
	{ s0,__,__,__, __,__,s0,__, __,__,__,__, __,__,__,__ },
	};

}

constexpr int track_count = 3;
static constexpr char tracks[][track_count] =
{
{ 0, 0, 0 },
{ 2, 10, 1 },
{ 3, 11, 1 },
{ 4, 12, 1 },
{ 5, 13, 1 },
{ 6, 14, 1 },
{ 7, 15, 1 },
{ 8, 16, 1 },
{ 9, 17, 1 }
};
constexpr int block_count = sizeof(tracks) / track_count;

constexpr fixed freq_to_omega(float f) { return (2 * pi * f) / sample_freq; }

fixed omega(note n)
{
	static constexpr fixed tuning[] =
	{
		freq_to_omega(32.703*32),
		freq_to_omega(34.648*32),
		freq_to_omega(36.708*32),
		freq_to_omega(38.891*32),
		freq_to_omega(41.203*32),
		freq_to_omega(43.654*32),
		freq_to_omega(46.249*32),
		freq_to_omega(48.999*32),
		freq_to_omega(51.913*32),
		freq_to_omega(55.000*32),
		freq_to_omega(58.270*32),
		freq_to_omega(61.735*32)
	};
	const auto oct = char(n) / 12;
	const auto semi = char(n) % 12;
	return fixed::from_code(code(tuning[semi]) >> (7 - oct));
}



struct instrument
{
	virtual fixed sample() = 0;
	virtual void strike(note, fixed) = 0;
};


struct sine_osc : instrument
{
	fixedcx gamma;
	fixedcx phi;

	sine_osc() : gamma{ 0, 0 }, phi{ 0, 0 } {};
	sine_osc(fixed omega, fixed gain = 0.9999) :
		gamma{ iexp(omega)*gain }, phi{ 1, 0 } { }

	fixed sample() final
	{
		const auto result = phi.im;
		phi = phi * gamma;
		return result;
	}

	void strike(note n, fixed g) final
	{
		gamma = iexp(omega(n))*g;
		phi = { 1, 0 };
	}
};

struct gain_decay
{
	fixed gain;
	fixed decay;
};

constexpr int relf_scale = 3;
constexpr fixed to_relf(double rf) { return rf / (1<<relf_scale); }
struct drum : instrument
{
	static constexpr int mode_count = 12;

	 //"ideal"
	static constexpr fixed relfs[mode_count] =
	{
		to_relf(1.00), to_relf(2.30), to_relf(3.60), // 0,1 - 0,2 - 0,3
		to_relf(1.59), to_relf(2.14), to_relf(2.63), // 1,1 - 2,1 - 3,1
		to_relf(3.16), to_relf(3.65), to_relf(4.15), // 4,1 - 5,1 - 6,1
		to_relf(2.92), to_relf(3.49), to_relf(4.06)  // 1,2 - 2,2 - 3,2
	};
	// some tom
	//static constexpr int relfs[mode_count] =
	//{
	//	to_relf(1.00), to_relf(1.85), to_relf(2.10), to_relf(2.29),
	//	to_relf(3.01), to_relf(3.23), to_relf(3.44), to_relf(3.50),
	//	to_relf(3.60), to_relf(3.65), to_relf(4.06), to_relf(4.15)
	//};


	sine_osc modes[mode_count];
	const gain_decay* mode_amps;

	drum(const gain_decay* mode_amps) : mode_amps(mode_amps) { }

	fixed sample() final
	{
		fixed result = 0;
		for (int i = 0; i < mode_count; ++i)
			result += modes[i].sample();
		return result;
	}

	void strike(note n, fixed a) final
	{
		auto omega = ::omega(n + 12*relf_scale);
		for (int i = 0; i < mode_count; ++i)
		{
			modes[i].gamma = iexp(omega * relfs[i]) * mode_amps[i].decay;
			modes[i].phi = { a*mode_amps[i].gain, 0 };
		}
	}
};

static constexpr gain_decay kick_modes[drum::mode_count] =
{
	{0, 0}, {0, 0}, {0, 0},
	{1.00, 0.9995}, {0.27, 0.9995}, {0.14, 0.999},
	{0.17, 0.999}, {0.10, 0.999}, {0.08, 0.999},
	{0, 0}, {0, 0}, {0, 0},
};


static constexpr fixed piano_harm[] = 
{
	1,
	0.357502966619163,
	0.104782052192043,
	0.147622056118452,
	0.070653804126245,
	0.024190383307069,
	0.212747712905242,
	0.029630668517408,
	0.041665159624309,
	0.029096747816013,
	0.019835474772456,
	0.00630893568336 ,
	0.033565119676382,
	0.018308048320845,
	0.006728487501151,
	0.008106922102421
};

struct harmonics : instrument
{
	fixedcx gamma;
	fixedcx phi;
	fixed gain;
	fixed ampl;
	fixed ogain;
	fixed oampl;

	const fixed* harm;

	harmonics(const fixed* harm, fixed omega, fixed gain = 0.9999) : 
		gamma{ iexp(omega) }, phi{ 1, 0 }, gain(gain), ampl(0), ogain(1), oampl(1), harm(harm) { }

	fixed sample() final
	{
		fixed acc = harm[0] * phi.im;
		auto ksi = phi;
		auto psi = phi;
		for (int k = 2; k < 17; ++k)
		{
			psi = oampl * (psi * ksi);
			acc = acc + harm[k - 1] * psi.im;
		}
		const auto result = acc * ampl;
		ampl = ampl * gain;
		oampl = oampl * ogain;
		phi = phi * gamma;
		return result;
	}

	void strike(note n, fixed a) final
	{
		gamma = iexp(omega(n))*0.9999999;
		phi = { 1, 0 };
		ampl = a;
		oampl = 1;

	}
};

struct lopass
{
	const fixed alpha;
	fixed y1 = 0;

	static constexpr fixed alpha_from_fc(double fc)
	{
		return 1 / (1 + sample_freq / (2*pi*fc));
	}

	fixed filter(fixed in)
	{
		y1 += alpha * (in - y1);
		return y1;
	}
};

void terminate(int code);
void* alloc(int bytes);
void play(float* buffer, int length, int sample_freq);


int __cdecl main()
{
	constexpr int buffer_len = beat_len * pat_len * (block_count+1);

	float* const buffer = (float*)alloc(buffer_len * sizeof(float));
	float* out = buffer;

	for (int i = 0; i < beat_len*pat_len; ++i)
		out[i] = 0;
	out += beat_len * pat_len;

	lopass master_lopass = { lopass::alpha_from_fc(10000) };

	harmonics osc1{ piano_harm, (2 * 3.141592f * 440) / sample_freq };
	harmonics osc2{ piano_harm, (2 * 3.141592f * 440) / sample_freq };

	drum drum1(kick_modes);

	instrument* const track_instrument[track_count] =
	{
		&osc1, &osc2, &drum1
	};
	const fixed track_gain[track_count] =
	{
		0.1, 0.1, 0.5
	};


	osc1.gain = 0.99995;
	osc1.ogain = 0.999999;
	osc2.gain = 0.99995;
	osc2.ogain = 0.999999;

	for (int bi = 0; bi < block_count; ++bi)
	{
		for (int step = 0; step < pat_len; ++step)
		{
			for (int ti = 0; ti < track_count; ++ti)
				if (auto n = melody::patterns[tracks[bi][ti]][step]; n != note::none)
					track_instrument[ti]->strike(n, track_gain[ti]);

			for (int sample = 0; sample < beat_len; ++sample, ++out)
			{
				fixed mix = 0;
				for (int ti = 0; ti < track_count; ++ti)
					mix += track_instrument[ti]->sample();
				*out = float(master_lopass.filter(mix));
			}
		}
	}

	play(buffer, buffer_len, sample_freq);

	terminate(0);
}