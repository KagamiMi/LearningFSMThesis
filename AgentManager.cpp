#include "AgentManager.h"

AgentManager::AgentManager(float basicMoveChange)
{
    //speed *= speedFactor;
    //jumpSpeed *= speedFactor;
    //immunityTime /= speedFactor;
    immunityMaxSteps = int(6.0 / basicMoveChange);
}

bool AgentManager::stillRunning()
{
    return ((health > 0) && !finished);
}

int AgentManager::getHealth()
{
    return health;
}

Record AgentManager::getRecord(int steps, float distanceX, float distanceY)
{

    return Record(points, health, enemiesDefeated, finished, steps,distanceX,distanceY);
}

float AgentManager::getXChange()
{
    return xChange;
}

float AgentManager::getYChange()
{
    return yChange;
}

void AgentManager::addToXChange(float change)
{
    xChange += change;
}

//float AgentManager::addToXChangeTime(float deltaTime)
//{
//    xChange += speed * deltaTime;
//    return xChange;
//}

void AgentManager::addToYChange(float change)
{
    yChange += change;
}

//float AgentManager::addToYChangeTime(float deltaTime)
//{
//    yChange += speed * deltaTime;
//    return yChange;
//}

void AgentManager::cleanXYChange()
{
    xChange = 0.0f;
    yChange = 0.0f;
}

void AgentManager::addPoints(int pointsToAdd)
{
    points += pointsToAdd;
}

int AgentManager::getPoints()
{
    return points;
}

void AgentManager::updateImmunity()
{
    if (isImmune)
    {
        passedImmunitySteps++;
        if (passedImmunitySteps >= immunityMaxSteps)
        {
            passedImmunitySteps = 0;
            isImmune = false;
        }
    }
}

void AgentManager::enemyDefeat()
{
    enemiesDefeated++;
}

bool AgentManager::getIsImmune()
{
    return isImmune;
}

void AgentManager::enemyAttack(int enemyDamage)
{
    health -= enemyDamage;
    isImmune = true;
    //printf("Health: %d \n", health);
}

void AgentManager::setFinished(bool finished)
{
    this->finished = finished;
}

float AgentManager::handleJumping(float moveChange, bool top, bool bottom)
{
    if (isJumping)
    {
        if (top) //podczas skoku dosiêgniêcie przeszkody z ty³u
        {
            isJumping = false;
            currentJumpDistance = 0;
        }
        else if (bottom)
        {
            isJumping = false;
            currentJumpDistance = 0;
        }
        else if (currentJumpDistance >= jumpDistance) //normalny koniec skoku
        {
            isJumping = false;
            currentJumpDistance = 0;
        }
        else //nadal skaczemy
        {
            yChange += moveChange * 2.0f;
            currentJumpDistance += yChange;
            return moveChange * 2.0f;

            //yChange += jumpSpeed * deltaTime;
            //currentJumpDistance += yChange;
            //return jumpSpeed * deltaTime;
        }
    }
    return 0;
}

float AgentManager::startJumping(float moveChange, bool top, bool bottom, bool* spacePressed)
{
    if (!isJumping && !top && *spacePressed && bottom)
    //if (!isJumping && !top && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && bottom) //pocz¹tek skoku
    {
        isJumping = true;
        yChange += moveChange * 2.0f;
        currentJumpDistance += yChange;
        return moveChange * 2.0f; 
        /*yChange += jumpSpeed * deltaTime;
        currentJumpDistance += yChange;
        return jumpSpeed * deltaTime;*/
    }
    return 0;
}

bool AgentManager::getIsJumping()
{
    return isJumping;
}
