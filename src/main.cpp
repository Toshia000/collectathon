#include <bn_core.h>
#include <bn_display.h>
#include <bn_log.h>
#include <bn_keypad.h>
#include <bn_random.h>
#include <bn_rect.h>
#include <bn_sprite_ptr.h>
#include <bn_sprite_text_generator.h>
#include <bn_size.h>
#include <bn_string.h>
#include <bn_backdrop.h>
#include <bn_timer.h>
#include <bn_timers.h>

#include "bn_sprite_items_smiley.h"
#include "bn_sprite_items_dot.h"
#include "bn_sprite_items_square.h"
#include "bn_sprite_items_watermelon_8x8.h"
#include "common_fixed_8x16_font.h"
#include <bn_sprite_palette_ptr.h>

// Pixels / Frame smiley moves at
static constexpr bn::fixed SPEED = 1.5;

// Amount of speed boost on top of base speed
static constexpr bn::fixed SPEED_BOOST = 3;

// Number of boosts
static constexpr int BOOST_COUNT = 3;

// Duration for the boost
static constexpr int BOOST_TIME = 3;

// Width and height of the the smiley and treasure bounding boxes
static constexpr bn::size PLAYER_SIZE = {8, 8};
static constexpr bn::size TREASURE_SIZE = {8, 8};

// Full bounds of the screen
static constexpr int MIN_Y = -bn::display::height() / 2;
static constexpr int MAX_Y = bn::display::height() / 2;
static constexpr int MIN_X = -bn::display::width() / 2;
static constexpr int MAX_X = bn::display::width() / 2;

// Number of characters required to show the longest numer possible in an int (-2147483647)
static constexpr int MAX_SCORE_CHARS = 11;

// Number of character required to show "boosts: " text and remaining boosts number (Boosts: 3)
static constexpr int MAX_BOOST_CHARS = 9;

// Number of character required to show "timer: " text and countdown in seconds (Timer: 3)
static constexpr int MAX_TIMER_CHARS = 8;

// Score location
static constexpr int SCORE_X = 70;
static constexpr int SCORE_Y = -70;

// Boost location
static constexpr int BOOST_X = -100;
static constexpr int BOOST_Y = -70;

// Timer location
static constexpr int TIMER_X = -20;
static constexpr int TIMER_Y = -70;

// Initial smiley location
static constexpr int INITIAL_PLAYER_X = 0;
static constexpr int INITIAL_PLAYER_Y = 0;

// Initial treasure location
static constexpr int INITIAL_TREASURE_X = 100;
static constexpr int INITIAL_TREASURE_Y = 0;

// Change smiley color every 30 frames
static constexpr int COLOR_CHANGE_FRAMES = 30;

int main()
{
    bn::core::init();
    bn::random rng = bn::random();

    bn::backdrop::set_color(bn::color(0, 0, 31));

    // Will hold the sprites for the score
    bn::vector<bn::sprite_ptr, MAX_SCORE_CHARS> score_sprites = {};

    // Will hold the sprites for the boost
    bn::vector<bn::sprite_ptr, MAX_BOOST_CHARS> boost_sprites = {};

    // Will hold the sprites for the timer
    bn::vector<bn::sprite_ptr, MAX_TIMER_CHARS> timer_sprites = {};

    bn::string<MAX_BOOST_CHARS> boosts_text = "Boosts: ";

    bn::string<MAX_TIMER_CHARS> timer_text = "Timer: ";

    bn::sprite_text_generator text_generator(common::fixed_8x16_sprite_font);

    int score = 0;

    bn::fixed speed_boost = 0;
    int boosts_left = BOOST_COUNT;
    int elapsed_seconds = 0;
    bool boost = false;

    bn::timer timer;

    int color_timer = 0;
    int current_color = 0;

    // Player and treasure sprites
    bn::sprite_ptr smiley = bn::sprite_items::smiley.create_sprite(INITIAL_PLAYER_X, INITIAL_PLAYER_Y);
    bn::sprite_ptr treasure = bn::sprite_items::watermelon_8x8.create_sprite(INITIAL_TREASURE_X, INITIAL_TREASURE_Y);

    bn::sprite_palette_ptr player_palette = smiley.palette(); // For alternating colors
    player_palette.set_color(9, bn::color(0, 31, 0));         // smiley will start green

    while (true)
    {
        color_timer++;

        if (color_timer >= COLOR_CHANGE_FRAMES)
        {
            color_timer = 0;
            current_color++;

            if (current_color > 3)
            {
                current_color = 0;
            }

            if (current_color == 0)
            {
                player_palette.set_color(9, bn::color(0, 31, 0)); // Green
            }
            else if (current_color == 1)
            {
                player_palette.set_color(9, bn::color(31, 31, 31)); // White
            }
            else if (current_color == 2)
            {
                player_palette.set_color(9, bn::color(31, 31, 0)); // Yellow
            }
            else if (current_color == 3)
            {
                player_palette.set_color(9, bn::color(31, 0, 0)); // Red
            }
        }

        if (bn::keypad::start_pressed())
        {
            smiley.set_position(INITIAL_PLAYER_X, INITIAL_PLAYER_Y);
            treasure.set_position(INITIAL_TREASURE_X, INITIAL_TREASURE_Y);
            score = 0;
            boosts_left = BOOST_COUNT;
            boost = false;
            speed_boost = 0;
            elapsed_seconds = 0;
        }

        if (bn::keypad::a_pressed() && boosts_left > 0 && !boost)
        {
            boost = true;
            boosts_left--;
            speed_boost = SPEED_BOOST;
            timer.restart();
        }

        if (boost)
        {
            elapsed_seconds = timer.elapsed_ticks() / bn::timers::ticks_per_second();
        }

        if (elapsed_seconds >= BOOST_TIME)
        {
            speed_boost = 0;
            boost = false;
            elapsed_seconds = 0;
        }

        // Change background color when boost is active
        if (boost)
        {
            bn::backdrop::set_color(bn::color(20, 0, 25)); // Purple when boost is active
        }
        else
        {
            bn::backdrop::set_color(bn::color(0, 0, 31)); // Blue (default)
        }

        // Move smiley with d-pad
        if (bn::keypad::left_held())
        {
            smiley.set_x(smiley.x() - SPEED - speed_boost);
        }
        if (bn::keypad::right_held())
        {
            smiley.set_x(smiley.x() + SPEED + speed_boost);
        }
        if (bn::keypad::up_held())
        {
            smiley.set_y(smiley.y() - SPEED - speed_boost);
        }
        if (bn::keypad::down_held())
        {
            smiley.set_y(smiley.y() + SPEED + speed_boost);
        }

        // Move smiley to opposite side of the screen when the smiley is out of bounds
        if (smiley.x() > MAX_X)
        {
            smiley.set_x(MIN_X);
        }
        if (smiley.x() < MIN_X)
        {
            smiley.set_x(MAX_X);
        }
        if (smiley.y() > MAX_Y)
        {
            smiley.set_y(MIN_Y);
        }
        if (smiley.y() < MIN_Y)
        {
            smiley.set_y(MAX_Y);
        }

        // The bounding boxes of the smiley and treasure, snapped to integer pixels
        bn::rect player_rect = bn::rect(smiley.x().round_integer(),
                                        smiley.y().round_integer(),
                                        PLAYER_SIZE.width(),
                                        PLAYER_SIZE.height());
        bn::rect treasure_rect = bn::rect(treasure.x().round_integer(),
                                          treasure.y().round_integer(),
                                          TREASURE_SIZE.width(),
                                          TREASURE_SIZE.height());

        // If the bounding boxes overlap, set the treasure to a new location an increase score
        if (player_rect.intersects(treasure_rect))
        {
            // Jump to any random point in the screen
            int new_x = rng.get_int(MIN_X, MAX_X);
            int new_y = rng.get_int(MIN_Y, MAX_Y);
            treasure.set_position(new_x, new_y);

            score++;
        }

        // Update score display
        bn::string<MAX_SCORE_CHARS> score_string = bn::to_string<MAX_SCORE_CHARS>(score);
        score_sprites.clear();
        text_generator.generate(SCORE_X, SCORE_Y,
                                score_string,
                                score_sprites);

        // Update boost display
        bn::string<MAX_BOOST_CHARS> boost_string = boosts_text + bn::to_string<MAX_BOOST_CHARS>(boosts_left);
        boost_sprites.clear();
        text_generator.generate(BOOST_X, BOOST_Y,
                                boost_string,
                                boost_sprites);

        // Update timer display
        timer_sprites.clear();

        if (boost)
        {
            bn::string<MAX_TIMER_CHARS> timer_string = timer_text + bn::to_string<MAX_TIMER_CHARS>(BOOST_TIME - elapsed_seconds);
            text_generator.generate(TIMER_X, TIMER_Y, timer_string, timer_sprites);
        }

        // Update RNG seed every frame so we don't get the same sequence of positions every time
        rng.update();

        bn::core::update();
    }
}