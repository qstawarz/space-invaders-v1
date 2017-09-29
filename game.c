#include "game.h"


bool init_game(t_game* game)
{
	if (game == NULL) return false;
	
	game->run = true;
	game->pause = false;
	game->start = false;
	game->replay = false;
	game->enemyPause = false;
	game->stage = 1;
	
	return initAll(&game->display) & 
		   initPlayer(&game->player, &game->display) & 
		   initEnemy(&game->enemy, &game->display);
}

void game_run(t_game* game)
{
	if (game == NULL) return;

	while (game->run == true)
	{
		clear_display(&game->display);

		checkInput(game);

		SDL_PumpEvents();

		if (game->player.life > 0 && game->enemy.life > 0 && game->pause == false && 
			game->start == true)
		{
			movePlayer(SDL_GetKeyboardState(NULL), &game->player);
			shootPlayer(SDL_GetKeyboardState(NULL), 
						&game->player, &game->display);
		}
		drawPlayer(&game->player, &game->display);
		if (game->player.bullet != NULL)
		{
			drawBullet(game->player.bullet, &game->display);
		}

		if (game->enemy.life > 0 && game->player.life > 0 && game->pause == false && 
			game->start == true && game->enemyPause == false)
		{
			moveEnemy(&game->enemy);
			shootEnemy(&game->enemy, &game->display);
		}
		drawEnemy(&game->enemy, &game->display);
		if (game->enemy.bullet != NULL)
		{
			drawBullet(game->enemy.bullet, &game->display);
		}
		
		collision(game);	

		displayPlayerLife(game);
		displayEnemyLife(game);
		displayGameStage(game);

		if (game->player.life <= 0 && game->enemy.life > 0)
		{
			game->player.bullet = NULL;
			game->player.speed = 0;
			game->enemy.bullet = NULL;
			game->enemy.speed = 0;
			displayGameOver(game);
			displayGameReplay(game);
		}
		if (game->enemy.life <= 0 && game->player.life > 0)
		{
			destroy_enemy(&game->enemy);
			destroy_player(&game->player);

			initPlayer(&game->player, &game->display);
			initEnemy(&game->enemy, &game->display);

			game->stage++;
			game->enemy.speed += (game->stage/5);

			game->start = false;
		}

		if (game->pause == true)
		{
			displayGamePause(game);
		}

		if (game->replay == true)
		{
			game->stage = 1;
			
			destroy_enemy(&game->enemy);
			destroy_player(&game->player);
		
			initPlayer(&game->player, &game->display);
			initEnemy(&game->enemy, &game->display);

			game->start = false;
			game->replay = false;
		}
		
		update_display(&game->display);
	}
}

void checkInput(t_game* game)
{
	if (game == NULL) return;

	SDL_Event event;

    while (SDL_PollEvent(&event) != 0)
    {
        if (event.type == SDL_QUIT)
        {
            game->run = false;
            break;
        }
        else if (event.type == SDL_KEYDOWN)
        {
            switch (event.key.keysym.sym)
            {
                case SDLK_ESCAPE:
                    game->run = false;
                    break;
				case SDLK_p :
					game->pause = true;
					break;
				case SDLK_y :
					if (game->player.life <= 0 || game->enemy.life <= 0)
					{
						game->replay = true;
					}
					break;
				case SDLK_n :
					if (game->player.life <= 0 || game->enemy.life <= 0)
					{
						game->run = false;
					}		
					break;
				default:
					game->start = true;
					game->pause = false;
                    break;
            }
        }
    }

}

void clean_game(t_game* game)
{
	if (game == NULL) return;

	destroy_bullet(game->enemy.bullet);
	destroy_bullet(game->player.bullet);
	destroy_enemy(&game->enemy);
	destroy_player(&game->player);
	clean_display(&game->display);
}

void collision(t_game* game)
{
	if (game == NULL) return;

	if (game->player.bullet != NULL)
	{
		if ((game->player.bullet->rect.x < game->enemy.rect.x + 60 && 
			game->player.bullet->rect.x > game->enemy.rect.x + 10) &&
			(game->player.bullet->rect.y > game->enemy.rect.y))
		{
			game->enemyPause = true;
		}
		if ((game->player.bullet->rect.x < game->enemy.rect.x + 50 && 
			game->player.bullet->rect.x > game->enemy.rect.x - 5) && 
			(game->player.bullet->rect.y < game->enemy.rect.y + 50 && 
			game->player.bullet->rect.y > game->enemy.rect.y))
		{
			destroy_bullet(game->player.bullet);
			game->player.bullet = NULL;
			game->enemy.life--;
	
			if (game->enemy.life == 0)
			{
				destroy_enemy(&game->enemy);
			}
		}
	}
	else
	{
		game->enemyPause = false;
	}

	if (game->enemy.bullet != NULL)
	{
		if ((game->enemy.bullet->rect.x < game->player.rect.x + 50 && 
			game->enemy.bullet->rect.x > game->player.rect.x - 5) && 
			(game->enemy.bullet->rect.y < game->player.rect.y + 50 && 
			game->enemy.bullet->rect.y > game->player.rect.y))
		{
			destroy_bullet(game->enemy.bullet);
			game->enemy.bullet = NULL;
			game->player.life--;

			if (game->player.life == 0)
			{
				destroy_player(&game->player);
			}
		}
	}
	if (game->enemy.rect.y >= 380)
	{
		game->player.life = 0;
	}
	if (game->enemy.bullet != NULL && game->player.bullet != NULL)
	{
		if ((game->enemy.bullet->rect.x < game->player.bullet->rect.x + 5 && 
			game->enemy.bullet->rect.x > game->player.bullet->rect.x - 5) && 
			(game->enemy.bullet->rect.y < game->player.bullet->rect.y + 5 && 
			game->enemy.bullet->rect.y > game->player.bullet->rect.y - 5))
		{
			destroy_bullet(game->enemy.bullet);
			game->enemy.bullet = NULL;
			destroy_bullet(game->player.bullet);
			game->player.bullet = NULL;
		}
	}
}

void displayPlayerLife(t_game* game)
{
	if (game == NULL) return;

    SDL_Color white = {255, 255, 255};

    char playerLife[512] = "Player Life : ";
    char* lifeNb = int_to_alpha(game->player.life);
    string_safe_concatenate(playerLife, lifeNb, 20);

    SDL_Surface* life = TTF_RenderUTF8_Solid(game->display.police, playerLife, white);
    SDL_Texture* lifeTex = SDL_CreateTextureFromSurface(game->display.renderer, life);

    int life_w, life_h;
    SDL_QueryTexture(lifeTex, NULL, NULL, &life_w, &life_h);

    SDL_Rect lifePos = {7, 7, life_w, life_h};

    SDL_RenderCopy(game->display.renderer, lifeTex, NULL, &lifePos);

    free(lifeNb);
    SDL_FreeSurface(life);
    SDL_DestroyTexture(lifeTex);
}

void displayEnemyLife(t_game* game)
{
	if (game == NULL) return;

	SDL_Color white = { 255, 255, 255 };

	char enemyLife[512] = "Enemy Life : ";
	char* lifeNb = int_to_alpha(game->enemy.life);
	string_safe_concatenate(enemyLife, lifeNb, 20);

	SDL_Surface* life = TTF_RenderUTF8_Solid(game->display.police, enemyLife, white);
	SDL_Texture* lifeTex = SDL_CreateTextureFromSurface(game->display.renderer, life);

	int life_w, life_h;
	SDL_QueryTexture(lifeTex, NULL, NULL, &life_w, &life_h);

	SDL_Rect lifePos = { 523, 7, life_w, life_h };

	SDL_RenderCopy(game->display.renderer, lifeTex, NULL, &lifePos);

	free(lifeNb);
	SDL_FreeSurface(life);
	SDL_DestroyTexture(lifeTex);
}

void displayGameOver(t_game* game)
{
	if (game == NULL) return;

	SDL_Color white = { 255, 255, 255 };

	char gameOver[512] = "GAME OVER";

	SDL_Surface* gameOverSur = TTF_RenderUTF8_Solid(game->display.police, gameOver, white);
	SDL_Texture* gameOverTex = SDL_CreateTextureFromSurface(game->display.renderer, gameOverSur);

	int gameOver_w, gameOver_h;
	SDL_QueryTexture(gameOverTex, NULL, NULL, &gameOver_w, &gameOver_h);

	SDL_Rect gameOverPos = { 320 - gameOver_w/2 - 100, 240 - gameOver_h/2 - 50, gameOver_w + 200, gameOver_h  + 100};

	SDL_RenderCopy(game->display.renderer, gameOverTex, NULL, &gameOverPos);

	SDL_FreeSurface(gameOverSur);
	SDL_DestroyTexture(gameOverTex);
}

void displayGamePause(t_game* game)
{
	if (game == NULL) return;

	SDL_Color white = { 255, 255, 255 };

	char gamePause[512] = "PAUSE";

	SDL_Surface* gamePauseSur = TTF_RenderUTF8_Solid(game->display.police, gamePause, white);
	SDL_Texture* gamePauseTex = SDL_CreateTextureFromSurface(game->display.renderer, gamePauseSur);

	int gamePause_w, gamePause_h;
	SDL_QueryTexture(gamePauseTex, NULL, NULL, &gamePause_w, &gamePause_h);

	SDL_Rect gamePausePos = { 320 - gamePause_w / 2 - 100, 240 - gamePause_h / 2 - 50, gamePause_w + 200, gamePause_h + 100 };

	SDL_RenderCopy(game->display.renderer, gamePauseTex, NULL, &gamePausePos);

	SDL_FreeSurface(gamePauseSur);
	SDL_DestroyTexture(gamePauseTex);
}

void displayGameReplay(t_game* game)
{
	if (game == NULL) return;

	SDL_Color white = { 255, 255, 255 };

	char gameReplay[512] = "REPLAY : Y / N";

	SDL_Surface* gameReplaySur = TTF_RenderUTF8_Solid(game->display.police, gameReplay, white);
	SDL_Texture* gameReplayTex = SDL_CreateTextureFromSurface(game->display.renderer, gameReplaySur);

	int gameReplay_w, gameReplay_h;
	SDL_QueryTexture(gameReplayTex, NULL, NULL, &gameReplay_w, &gameReplay_h);

	SDL_Rect gamePausePos = { 320 - gameReplay_w / 2 - 25, 300 - gameReplay_h / 2 - 12.5, gameReplay_w + 50, gameReplay_h + 25 };

	SDL_RenderCopy(game->display.renderer, gameReplayTex, NULL, &gamePausePos);

	SDL_FreeSurface(gameReplaySur);
	SDL_DestroyTexture(gameReplayTex);
}

void displayGameStage(t_game* game)
{
	if (game == NULL) return;

	SDL_Color white = { 255, 255, 255 };

	char gameStage[512] = "Stage : ";
	char* stageNb = int_to_alpha(game->stage);
	string_safe_concatenate(gameStage, stageNb, 20);

	SDL_Surface* stage = TTF_RenderUTF8_Solid(game->display.police, gameStage, white);
	SDL_Texture* stageTex = SDL_CreateTextureFromSurface(game->display.renderer, stage);

	int stage_w, stage_h;
	SDL_QueryTexture(stageTex, NULL, NULL, &stage_w, &stage_h);

	SDL_Rect stagePos = {(640 - stage_w)/2, 7, stage_w, stage_h };

	SDL_RenderCopy(game->display.renderer, stageTex, NULL, &stagePos);

	free(stageNb);
	SDL_FreeSurface(stage);
	SDL_DestroyTexture(stageTex);
}