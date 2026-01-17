#pragma once
#include <string_view>

namespace GameConstants
{
	namespace Screen
	{
		constexpr int WIDTH = 1920;
		constexpr int HEIGHT = 1080;
		constexpr int TARGET_FPS = 60;
	}
	namespace Player
	{
		constexpr float BASE_HEIGHT = 70.0f;
		constexpr float RADIUS = 50.0f;
		constexpr float SPEED = 7.0f;
		constexpr int INITIAL_LIVES = 3;

		namespace Rendering {
			constexpr float TEXTURE_SIZE = 100.0f;
			constexpr float TEXTURE_OFFSET = 50.0f;
			constexpr int TEXTURE_COUNT = 3;
			constexpr float SOURCE_SIZE = 352.0f;
		}

		namespace Animation
		{
			constexpr float INTERVAL = 0.4f;
		}

		namespace Shooting {
			constexpr float SPAWN_Y_OFFSET = 130.0f;
		}
	}
	namespace Projectile {

		constexpr int SPEED = 15;
		constexpr int OUT_OF_BOUNDS = 1500;

		namespace Rendering {

			constexpr float TEXTURE_SIZE = 50.f;
			constexpr float TEXTURE_OFFSET = 25.f;
			constexpr float SOURCE_SIZE = 176.0f;
		}
	}
	namespace Wall {

		constexpr int INITIAL_HEALTH = 50;
		constexpr int RADIUS = 60;
		constexpr float Y_OFFSET = 250.0f;

		namespace Rendering
		{
			constexpr float TEXTURE_SIZE = 200.f;
			constexpr float TEXTURE_OFFSET = 100.f;
			constexpr float SOURCE_SIZE = 704.0f;

			namespace HealthDisplay
			{
				constexpr int TEXT_OFFSET_X = 21;
				constexpr int TEXT_OFFSET_Y = 10;
				constexpr int TEXT_SIZE = 40;
			}

		}
	}
	namespace Alien {
		constexpr int SPEED = 2;
		constexpr float RADIUS = 30.0f;
		constexpr int DROP_DISTANCE = 50;

		namespace Rendering {
			constexpr float TEXTURE_SIZE = 100.f;
			constexpr float TEXTURE_OFFSET = 50.f;
			constexpr float SOURCE_SIZE = 352.0f;
		}

		namespace Shooting {
			constexpr float Y_OFFSET = 40.0f;
			constexpr int INTERVAL_FRAMES = 59;
			constexpr int PROJECTILE_SPEED = -15;
		}
	}

	namespace Formation {
		constexpr int WIDTH = 8;
		constexpr int HEIGHT = 5;
		constexpr int SPACING = 80;
		constexpr int START_X = 100;
		constexpr int START_Y = 50;
		constexpr int OFFSET_X = 450;
	}
	namespace Scoring {
		constexpr int POINTS_PER_ALIEN = 100;
	}
	namespace Background
	{
		constexpr int STAR_COUNT = 600;
		constexpr float PARALLAX_SPEED = 15.0f;

		namespace Stars {
			constexpr int MIN_SIZE = 1;
			constexpr int MAX_SIZE = 4;
			constexpr int SIZE_DIVISOR = 2;
			constexpr int SPAWN_OFFSET = 150;
		}
	}

	namespace UI {
		constexpr int MAX_NAME_LENGTH = 8;
		constexpr int LEADERBOARD_SIZE = 5;
		constexpr int TEXT_BLINK_INTERVAL = 20;

		namespace StartScreen {
			constexpr int TITLE_X = 200;
			constexpr int TITLE_Y = 100;
			constexpr int TITLE_SIZE = 160;
			constexpr int PROMPT_X = 200;
			constexpr int PROMPT_Y = 350;
			constexpr int PROMPT_SIZE = 40;
		}

		namespace HUD {
			constexpr int SCORE_X = 50;
			constexpr int SCORE_Y = 20;
			constexpr int LIVES_X = 50;
			constexpr int LIVES_Y = 70;
			constexpr int TEXT_SIZE = 40;
		}

		namespace EndScreen {
			constexpr int HIGHSCORE_X = 600;
			constexpr int HIGHSCORE_Y = 300;
			constexpr int HIGHSCORE_SIZE = 60;
			constexpr int CONTINUE_Y = 200;
			constexpr int CONTINUE_SIZE = 40;

			namespace NameEntry {
				constexpr int PROMPT_Y = 400;
				constexpr int PROMPT_SIZE = 20;
				constexpr int TEXTBOX_X = 600;
				constexpr int TEXTBOX_Y = 500;
				constexpr int TEXTBOX_WIDTH = 225;
				constexpr int TEXTBOX_HEIGHT = 50;
				constexpr int TEXT_X_OFFSET = 5;
				constexpr int TEXT_Y_OFFSET = 8;
				constexpr int TEXT_SIZE = 40;
				constexpr int COUNT_Y = 600;
				constexpr int BACKSPACE_PROMPT_Y = 650;
				constexpr int CONTINUE_PROMT_Y = 800;
			}
		}

		namespace Leaderboard {
			constexpr int TITLE_X = 50;
			constexpr int TITLE_Y = 100;
			constexpr int ROW_HEIGHT = 40;
			constexpr int START_Y = 140;
			constexpr int NAME_X = 50;
			constexpr int SCORE_X = 350;
		}
	}

	namespace Files {
		constexpr std::string_view LEADERBOARD_PATH = "./leaderboard.txt";
	}
}