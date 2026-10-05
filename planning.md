A place to write your findings and plans

## Understanding
- Speed is set to 1 pixel per frame
- Constexpr is a little confusing, because it is a new data type
- Player and treasure both have size of 8 by 8 pixels
- Screen bounds are set to the size of the viewport
- `MAX_SCORE_CHARS` is set to 11 characters. It is the maximum amount of characters in the score, but the value in the comment has only 10 characters and it is negative, so that is confusing
- Score location is little confusing because on the y axis negative number represents top part of the screen, and positive number represents bottom part
- `score_sprites` is container that can store 11 sprites
- `text generator` generates text sprites to display score
- Initial position of player is at the bottom left and the treasure is at the center
- While (true) loop runs every frame
- If player holds down up, right, down or left button, it moves player by set speed, which is 1 pixel
- Bounding boxes are created around the player and treasure, `round_integer()` is confusing, because I suppose the player position is integer
- When the player hits the treasure, treasure moves to a new place within the screen and score integer increases by 1
- Score is converted from the integer to string, and new sprite with new score is generated
- Seed for the random function is updated, so the treasure does not move to same sequence of positions

## Planning required changes
- Increase the speed of the player by changing speed variable
- Changed background color to blue
- Create new variables for player and treasure starting positions
- When enter key /start button is pressed, player/treasure positions reset and score is set to 0
- Player should apper on the opposite side of the screen when he is out of bounds of the screen, 
## Brainstorming game ideas

## Plan for implementing game

