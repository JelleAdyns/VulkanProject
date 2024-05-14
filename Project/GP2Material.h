#pragma once
#include "GP2Texture.h"
#include <memory>
struct GP2Material
{
	GP2Material() = default;
	~GP2Material() = default;

	GP2Material(const GP2Material& other) = delete;
	GP2Material(GP2Material&& other) noexcept = default;
	GP2Material& operator=(const GP2Material& other) = delete;
	GP2Material& operator=(GP2Material&& other) noexcept = default;

	GP2Texture* m_Diffuse;
	GP2Texture* m_Roughness;
	GP2Texture* m_Specular;
	GP2Texture* m_Normal;
};