#include <type_traits>

constexpr double pi = 3.1415926535;

template <class T, class R>
using if_integral_t = std::enable_if_t<std::is_integral_v<T>, R>;
template <class T, class R>
using if_arithmetic_t = std::enable_if_t<std::is_arithmetic_v<T>, R>;

//enum class fixed : int { zero, half = 1<<29, one = 1<<30, two = 1<<31 };
//
//constexpr int code(fixed x) { return static_cast<int>(x); }
//
//
//constexpr fixed fix(double x) { return static_cast<fixed>(static_cast<int>(x*code(fixed::one))); }
//constexpr float unfix(fixed x) { return code(x) / float(code(fixed::one)); }
//
//constexpr fixed operator+(fixed x) { return x; }
//constexpr fixed operator-(fixed x) { return static_cast<fixed>(-code(x)); }
//
//constexpr fixed operator+(fixed a, fixed b) { return static_cast<fixed>(code(a) + code(b)); }
//constexpr fixed operator-(fixed a, fixed b) { return static_cast<fixed>(code(a) - code(b)); }
//
//template <class T> constexpr if_integral_t<T, fixed> operator*(T a, fixed b) { return static_cast<fixed>(a * code(b)); }
//template <class T> constexpr if_integral_t<T, fixed> operator*(fixed a, T b) { return static_cast<fixed>(code(a) * b); }
//template <class T> constexpr if_integral_t<T, fixed> operator/(fixed a, T b) { return static_cast<fixed>(code(a) / b); }
//
//constexpr fixed operator*(fixed a, fixed b)
//{
//	return static_cast<fixed>((static_cast<long long>(a) * static_cast<long long>(b)) >> 30);
//}

class fixed
{
	int _code = 0;
	static constexpr struct {} _from_code = {};

	constexpr fixed(decltype(_from_code), int c) : _code(c) { }
public:
	constexpr fixed(int c) : _code(c<<30) { }
	constexpr fixed(double x) : _code(int(x*(1<<30))) { }
	constexpr fixed(float x) : _code(int(x*(1<<30))) { }

	friend constexpr int code(fixed x) { return x._code; }
	static constexpr fixed from_code(int c) { return { _from_code, c }; }

	explicit constexpr operator float() const { return float(_code) / (1 << 30); }

	constexpr fixed operator-() const { return from_code( -_code ); }
	constexpr fixed operator+() const { return *this; }

	friend constexpr fixed operator+(fixed a, fixed b) { return from_code( a._code + b._code ); }
	friend constexpr fixed operator-(fixed a, fixed b) { return from_code( a._code - b._code ); }
	friend constexpr fixed operator*(fixed a, fixed b)
	{
		return from_code( (static_cast<long long>(a._code) * static_cast<long long>(b._code))>>30 );
	}
	friend constexpr fixed operator/(fixed a, fixed b)
	{
		return (float(a) / float(b));
	}
	template <class T>
	friend constexpr if_integral_t<T, fixed> operator*(T a, fixed b) { return from_code(a*b._code); }
	template <class T>
	friend constexpr if_integral_t<T, fixed> operator*(fixed a, T b) { return from_code(a._code*b); }
	template <class T>
	friend constexpr if_integral_t<T, fixed> operator/(fixed a, T b) { return from_code(a._code/b); }
};

struct fixedcx
{
	fixed re;
	fixed im;
};

fixedcx iexp(const fixed im)
{
	fixed p = im;
	fixedcx r = { 1, 0 };
	for (char k = 1; k < 3; k+=2)
	{
		r.im = r.im + p;
		p = -im * p / (k + 1);
		r.re = r.re + p;
		p = +im * p / (k + 2);
	}
	return r;
}


template <class T>
constexpr if_arithmetic_t<T, fixedcx> operator*(T a, fixedcx b) { return { a * b.re, a * b.im }; }
template <class T>
constexpr if_arithmetic_t<T, fixedcx> operator*(fixedcx a, T b) { return { a.re * b, a.im * b }; }
template <class T>
constexpr if_arithmetic_t<T, fixedcx> operator/(fixedcx a, T b) { return { a.re / b, a.im / b }; }


constexpr fixedcx operator*(fixedcx a, fixedcx b)
{
	return { a.re*b.re - a.im*b.im, a.re*b.im + a.im*b.re };
}
constexpr fixedcx operator*(fixed a, fixedcx b) { return { a*b.re, a*b.im }; }
constexpr fixedcx operator*(fixedcx a, fixed b) { return { a.re*b, a.im*b }; }
constexpr fixedcx operator/(fixedcx a, fixed b) { return { a.re/b, a.im/b }; }

enum class note : char { };
constexpr note NN = note(0);

constexpr note operator+(note a, char b) { return note(char(a) + b); }

constexpr int sample_freq = 48000;
constexpr int beat_len = (48000*60)/(120*4);
//constexpr float bpm = sample_frequency / 4.0f / step_length;
constexpr int pat_len = 16;


constexpr note key = NN + 48;

static constexpr note patterns[][pat_len] =
{
{ key+0,NN,NN,NN, key+2,NN,NN,NN, key+4,NN,NN,NN, key+5,NN,NN,NN },
{ key+7,NN,NN,NN, NN,NN,NN,NN, key+7,NN,NN,NN, NN,NN,NN,NN },
{ key+9,NN,NN,NN, key+9,NN,NN,NN, key+9,NN,NN,NN, key+9,NN,NN,NN },
{ key+7,NN,NN,NN, NN,NN,NN,NN, NN,NN,NN,NN, NN,NN,NN,NN },
{ key+5,NN,NN,NN, key+5,NN,NN,NN, key+5,NN,NN,NN, key+5,NN,NN,NN },
{ key+4,NN,NN,NN, NN,NN,NN,NN, key+4,NN,NN,NN, NN,NN,NN,NN },
{ key+2,NN,NN,NN, key+2,NN,NN,NN, key+2,NN,NN,NN, key+2,NN,NN,NN },
{ key,NN,NN,NN, NN,NN,NN,NN, NN,NN,NN,NN, NN, NN,NN,NN },
{ key + (-12),NN,NN,key + (-12), NN,NN,key + (-12),NN, NN,key + (-12),NN,NN, key + (-12),NN,key + (-12),NN },
{ key + (-5),NN,NN,key + (-5), NN,NN,key + (-5),NN, NN,key + (-5),NN,NN, key + (-5),NN,key + (-5),NN },
{ key + (-3),NN,NN,key + (-3), NN,NN,key + (-3),NN, NN,key + (-3),NN,NN, key + (-3),NN,key + (-3),NN },
{ key + (-5),NN,NN,NN, NN,NN,key + (-5),NN, NN,NN,NN,NN, key + (-5),NN,NN,NN },
{ key + (-7),NN,NN,key + (-7), NN,NN,key + (-7),NN, NN,key + (-7),NN,NN, key + (-7),NN,key + (-7),NN },
{ key + (-8),NN,NN,key + (-8), NN,NN,key + (-8),NN, NN,key + (-8),NN,NN, key + (-8),NN,key + (-8),NN },
{ key + (-10),NN,NN,key + (-10), NN,NN,key + (-10),NN, NN,key + (-10),NN,NN, key + (-10),NN,key + (-10),key + (-10) },
{ key + (-12),NN,NN,NN, NN,NN,key + (-12),NN, NN,NN,NN,NN, NN, NN,NN,NN }
};

constexpr int track_count = 2;
static constexpr char tracks[][track_count] =
{
{ 0, 8 },
{ 1, 9 },
{ 2, 10 },
{ 3, 11 },
{ 4, 12 },
{ 5, 13 },
{ 6, 14 },
{ 7, 15 }
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




struct sine_osc
{
	fixedcx gamma;
	fixedcx phi;

	sine_osc(fixed omega, fixed gain = 0.9999) :
		gamma{ iexp(omega)*gain }, phi{ 0, -1 } { }

	void render(float* buffer)
	{
		for (int i = 0; i < beat_len; ++i)
		{
			buffer[i] = float(phi.re);
			phi = phi * gamma;
		}
	}
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

struct harmonics
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

	void render(float* buffer)
	{
		for (int i = 0; i < beat_len; ++i)
		{
			fixed acc = harm[0]*phi.im;
			auto ksi = phi;
			auto psi = phi;
			for (int k = 2; k < 17; ++k)
			{
				psi = oampl*(psi * ksi);
				acc = acc + harm[k-1]*psi.im;
			}
			buffer[i] = float(acc*ampl);
			ampl = ampl * gain;
			oampl = oampl * ogain;
			phi = phi * gamma;
		}
	}

	void strike(note n, fixed a)
	{
		gamma = iexp(omega(n))*0.9999999;
		phi = { 1, 0 };
		ampl = a;
		oampl = 1;

	}
};

void terminate(int code);
void* alloc(int bytes);
void play(float* buffer, int length, int sample_freq);


int __cdecl main()
{
	constexpr int buffer_len = beat_len * pat_len * block_count;

	float* const buffer = (float*)alloc(buffer_len * sizeof(float));
	float* const step_buffer = (float*)alloc(beat_len*sizeof(float));
	float* out = buffer;

	harmonics osc1{ piano_harm, (2 * 3.141592f * 440) / sample_freq };
	harmonics osc2{ piano_harm, (2 * 3.141592f * 440) / sample_freq };
	harmonics osc3{ piano_harm, (2 * 3.141592f * 440) / sample_freq };

	osc1.gain = 0.99995;
	osc1.ogain = 0.999999;
	osc2.gain = 0.99995;
	osc2.ogain = 0.999999;

	for (int bi = 0; bi < block_count; ++bi)
	{
		const note* pats[track_count];
		for (int ti = 0; ti < track_count; ++ti)
			pats[ti] = patterns[tracks[bi][ti]];
		for (int si = 0; si < pat_len; ++si)
		{
			if (auto n = pats[0][si]; n != NN)
				osc1.strike(n, 0.6);
			if (auto n = pats[1][si]; n != NN)
				osc2.strike(n, 0.6);
			osc1.render(step_buffer);
			for (int i = 0; i < beat_len; ++i) out[i] = step_buffer[i];
			osc2.render(step_buffer);
			for (int i = 0; i < beat_len; ++i) out[i] += step_buffer[i];
			out += beat_len;
		}
	}

	play(buffer, buffer_len, sample_freq);

	terminate(0);
}