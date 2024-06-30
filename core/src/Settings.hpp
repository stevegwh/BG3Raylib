//
// Created by Steve Wheeler on 06/05/2024.
//

#pragma once

#include "cereal/cereal.hpp"


struct Settings
{
	int screenWidth = 1280;
	int screenHeight = 720;

	void ResetScreenSize()
	{
		screenWidth = SCREEN_WIDTH;
		screenHeight = SCREEN_HEIGHT;
	}

	template <class Archive>
	void save(Archive& archive) const
	{
		archive(CEREAL_NVP(SCREEN_WIDTH),
		        CEREAL_NVP(SCREEN_HEIGHT));
	}

	template <class Archive>
	void load(Archive& archive)
	{
		archive(CEREAL_NVP(SCREEN_WIDTH),
		        CEREAL_NVP(SCREEN_HEIGHT));
		screenWidth = SCREEN_WIDTH;
		screenHeight = SCREEN_HEIGHT;
	}

private:
	int SCREEN_WIDTH = 1280;
	int SCREEN_HEIGHT = 720;
};
