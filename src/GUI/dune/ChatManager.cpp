/*
 *  This file is part of Dune Legacy.
 *
 *  Dune Legacy is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Dune Legacy is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Dune Legacy.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <GUI/dune/ChatManager.h>

#include <FileClasses/FontManager.h>
#include <globals.h>

#include <ctime>
#include <algorithm>

#define MAX_MESSAGESHOWTIME     15000
#define MAX_NUMBEROFMESSAGES    25

ChatManager::ChatManager() : Widget()
{

}

ChatManager::~ChatManager()
{

}

void ChatManager::Draw(SDL_Surface* screen, Point Position)
{
    // delete all old messages
    Uint32 currentTime = SDL_GetTicks();
    while(chatMessages.empty() == false && chatMessages.front().messageTime + MAX_MESSAGESHOWTIME < currentTime) {
        chatMessages.pop_front();
    }

    // determine maximum vertical size of username and time
    int maxUsernameSizeY = 0;
    int maxTimeSizeY = 0;
    std::list<ChatMessage>::iterator iter;
    for(iter = chatMessages.begin(); iter != chatMessages.end(); ++iter) {
        if(iter->messageType == MSGTYPE_NORMAL) {
            maxUsernameSizeY = std::max(maxUsernameSizeY, iter->pUsernameSurface->w);
            maxTimeSizeY = std::max(maxTimeSizeY, iter->pTimeSurface->w);
        }
    }

    SDL_Rect timedest = { Position.x, Position.y, 0, 0};
    SDL_Rect usernamedest = { Position.x + 70, Position.y, 0, 0};
    SDL_Rect messagedest = { Position.x + 70 + maxUsernameSizeY, Position.y, 0, 0};
    for(iter = chatMessages.begin(); iter != chatMessages.end(); ++iter) {

        if(iter->messageType == MSGTYPE_NORMAL) {
            timedest.w = iter->pTimeSurface->w;
            timedest.h = iter->pTimeSurface->h;
            SDL_BlitSurface(iter->pTimeSurface.get(), NULL, screen, &timedest);

            usernamedest.w = iter->pUsernameSurface->w;
            usernamedest.h = iter->pUsernameSurface->h;
            usernamedest.x = Position.x + 70 + maxUsernameSizeY - iter->pUsernameSurface->w;
            SDL_BlitSurface(iter->pUsernameSurface.get(), NULL, screen, &usernamedest);

            messagedest.w = iter->pMessageSurface->w;
            messagedest.h = iter->pMessageSurface->h;
            SDL_BlitSurface(iter->pMessageSurface.get(), NULL, screen, &messagedest);


        } else {
            // MSGTYPE_INFO
            SDL_Rect infodest = messagedest;
            infodest.x = Position.x + 70 - 20;
            infodest.w = iter->pMessageSurface->w;
            messagedest.h = infodest.h = iter->pMessageSurface->h;

            SDL_BlitSurface(iter->pMessageSurface.get(), NULL, screen, &infodest);
        }

        timedest.y += messagedest.h;
        usernamedest.y += messagedest.h;
        messagedest.y += messagedest.h;
    }
}

void ChatManager::addChatMessage(std::string message, std::string username)
{
    char timestring[80];
    time_t unixtime = time(NULL);
    struct tm* timeinfo;

    timeinfo = localtime( &unixtime );
    strftime(timestring, 80, "(%H:%M:%S)", timeinfo);

    std::shared_ptr<SDL_Surface> pTimeSurface = std::shared_ptr<SDL_Surface>( pFontManager->createSurfaceWithText( timestring, COLOR_WHITE, FONT_STD10), SDL_FreeSurface);
    std::shared_ptr<SDL_Surface> pUsernameSurface = std::shared_ptr<SDL_Surface>( pFontManager->createSurfaceWithText( username + ": ", COLOR_WHITE, FONT_STD10), SDL_FreeSurface);
    std::shared_ptr<SDL_Surface> pMessageSurface = std::shared_ptr<SDL_Surface>( pFontManager->createSurfaceWithText( message, COLOR_WHITE, FONT_STD10), SDL_FreeSurface);

    chatMessages.push_back( ChatMessage(pTimeSurface, pUsernameSurface, pMessageSurface, SDL_GetTicks(), MSGTYPE_NORMAL) );

    // delete old messages if there are too many messages on the screen
    while(chatMessages.size() > MAX_NUMBEROFMESSAGES) {
        chatMessages.pop_front();
    }
}

void ChatManager::addInfoMessage(std::string message)
{
    std::shared_ptr<SDL_Surface> pMessageSurface = std::shared_ptr<SDL_Surface>( pFontManager->createSurfaceWithText( "*  " + message, COLOR_GREEN, FONT_STD10), SDL_FreeSurface);

    chatMessages.push_back( ChatMessage(pMessageSurface, SDL_GetTicks(), MSGTYPE_INFO) );

    // delete old messages if there are too many messages on the screen
    while(chatMessages.size() > MAX_NUMBEROFMESSAGES) {
        chatMessages.pop_front();
    }
}
