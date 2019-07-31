#pragma once

constexpr double pi = 3.1415926535;

template <class T, class R>
using if_integral_t = std::enable_if_t<std::is_integral_v<T>, R>;
template <class T, class R>
using if_arithmetic_t = std::enable_if_t<std::is_arithmetic_v<T>, R>;

class fixed
{
	static constexpr int SCALE = 30;
	int _code = 0;
	static constexpr struct {} _from_code = {};

	constexpr fixed(decltype(_from_code), int c) : _code(c) { }
public:
	constexpr fixed(int c) : _code(c << SCALE) { }
	constexpr fixed(double x) : _code(int(x* (1LL << SCALE))) { }
	constexpr fixed(float x) : _code(int(x* (1 << SCALE))) { }

	friend constexpr int code(fixed x) { return x._code; }
	static constexpr fixed from_code(int c) { return { _from_code, c }; }

	explicit constexpr operator float() const { return float(_code) / (1 << SCALE); }

	constexpr fixed operator-() const { return from_code(-_code); }
	constexpr fixed operator+() const { return *this; }

	constexpr void operator+=(fixed b) { _code += b._code; }

	friend constexpr fixed operator+(fixed a, fixed b) { return from_code(a._code + b._code); }
	friend constexpr fixed operator-(fixed a, fixed b) { return from_code(a._code - b._code); }
	friend constexpr fixed operator*(fixed a, fixed b)
	{
		return from_code((static_cast<long long>(a._code) * static_cast<long long>(b._code)) >> SCALE);
	}
	friend constexpr fixed operator/(fixed a, fixed b)
	{
		return (float(a) / float(b));
	}
	template <class T>
	friend constexpr if_integral_t<T, fixed> operator*(T a, fixed b) { return from_code(a * b._code); }
	template <class T>
	friend constexpr if_integral_t<T, fixed> operator*(fixed a, T b) { return from_code(a._code * b); }
	template <class T>
	friend constexpr if_integral_t<T, fixed> operator/(fixed a, T b) { return from_code(a._code / b); }
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
	for (char k = 1; k < 3; k += 2)
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
	return { a.re * b.re - a.im * b.im, a.re * b.im + a.im * b.re };
}
constexpr fixedcx operator*(fixed a, fixedcx b) { return { a * b.re, a * b.im }; }
constexpr fixedcx operator*(fixedcx a, fixed b) { return { a.re * b, a.im * b }; }
constexpr fixedcx operator/(fixedcx a, fixed b) { return { a.re / b, a.im / b }; }

