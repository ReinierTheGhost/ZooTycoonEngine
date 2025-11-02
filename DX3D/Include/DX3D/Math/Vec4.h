#pragma once
#include <DX3D/Core/Core.h>

namespace dx3d
{
	class Vec4
	{
	public:
		Vec4() = default;
		Vec4(f32 r, f32 g, f32 b, f32 a): r(r), g(g), b(b), a(a){}

	public:
		f32 r{}, g{}, b{}, a{};
	};
}
