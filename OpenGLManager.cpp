#include "OpenGLManager.h"

OpenGLManager::OpenGLManager(std::string mapPath, float basicMoveChange, AgentManager* agentManager, bool windowVisible) 
    : mapPath(mapPath), agentManager(agentManager), basicMoveChange(basicMoveChange), basicMoveChangeEnemy(basicMoveChange/2.0f), windowVisible(windowVisible)
{
    //gravity *= speedFactor;
    //enemySpeed *= speedFactor;
}

#pragma region initialization

int OpenGLManager::initialization()
{
    glewExperimental = true;

    //Inicjalizacja GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    //Ustawianie ró¿nych parametrów okien
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    if (!windowVisible)
    {
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    }

    //Utworzenie okna
    window = glfwCreateWindow(width, height, "FSM learning", NULL, NULL);
    if (!window)
    {
        fprintf(stderr, "Failed to open GLFW window. If you have Intel GPU, they are not 4.0 compatible.");
        glfwTerminate();
        return -1;
    }

    if (!windowVisible)
    {
        glfwHideWindow(window);
    }

    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glfwSetWindowPos(window, mode->width / 2 - width / 2, mode->height / 2 - height / 2);


    //Utworzenie kontekstu okna
    glfwMakeContextCurrent(window);

    //Inicjalizacja GLEW
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize GLEW\n");
        glfwTerminate();
        return -1;
    }

    //Ustawienie dla okna opcji interakcji wejœcia
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    

    //Wczytanie danych obiektów
    loadMap();
    loadPlatforms();
    loadAgent();
    loadCollectables();
    loadEnemies();
    loadFinish();

    programID = loadShaders("VertexShader.hlsl", "FragmentShader.hlsl");

    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

    
    float scaling = 0.01f;

    MatrixID = glGetUniformLocation(programID, "MVP");
    colorUniform = glGetUniformLocation(programID, "obj_color");

    Projection = glm::ortho(-1.0f * width * scaling, 1.0f * width * scaling, -1.0f * height * scaling, 1.0f * height * scaling, 0.0f, 100.0f);
    View = glm::lookAt(glm::vec3(0, 0, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

    vec4 viewport = vec4(0, 0, width, height);
    vec3 unprojected = glm::unProject(vec3(800, 500, 0), View * Model, Projection, viewport);

    worldSizeHalfWidth = unprojected.x;
    worldSizeHalfHeight = unprojected.y;


    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (windowVisible)
    {
        bool* boolTemp = new bool(false);
        Model = getModelMatrix(boolTemp, boolTemp, boolTemp);
        glm::mat4 MVPmatrix = Projection * View * Model;
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVPmatrix[0][0]);
        glUseProgram(programID);
        draw();
        glfwSwapBuffers(window);

        glfwPollEvents();

        Model = getModelMatrix(boolTemp, boolTemp, boolTemp);
        MVPmatrix = Projection * View * Model;
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVPmatrix[0][0]);
        glUseProgram(programID);
        draw();
        glfwSwapBuffers(window);

        glfwPollEvents();


        delete boolTemp;
    }
}

void OpenGLManager::loadMap()
{
    std::ifstream mapFileStream(mapPath, std::ios::in);

    if (mapFileStream.is_open())
    {
        std::string line;
        
        std::stringstream sstr;
        std::string str;

        std::getline(mapFileStream, line);
        sstr.str(line);
        sstr >> str;

        float xMin, xMax, yMin, yMax, xMinMoveLimit, xMaxMoveLimit;
        while (line.compare(""))
        {
            if (!str.compare("agent"))
            {
                while (std::getline(mapFileStream, line))
                {
                    sstr.clear();
                    sstr.str(line);
                    if (!(sstr >> xMin >> xMax >> yMin >> yMax))
                    {
                        sstr.clear();
                        sstr >> str;
                        break;
                    }
                    agent.element = Rectangle(xMin, xMax, yMin, yMax);
                }
            }
            else if (!str.compare("platforms"))
            {
                while (std::getline(mapFileStream, line))
                {
                    sstr.clear();
                    sstr.str(line);
                    if (!(sstr >> xMin >> xMax >> yMin >> yMax))
                    {
                        sstr.clear();
                        sstr >> str;
                        break;
                    }
                    platforms.element.push_back(Rectangle(xMin, xMax, yMin, yMax));
                }
            }
            else if (!str.compare("collectables"))
            {
                while (std::getline(mapFileStream, line))
                {
                    sstr.clear();
                    sstr.str(line);
                    if (!(sstr >> xMin >> xMax >> yMin >> yMax))
                    {
                        sstr.clear();
                        sstr >> str;
                        break;
                    }
                    collectables.element.push_back(Rectangle(xMin, xMax, yMin, yMax));
                }
            }
            else if (!str.compare("enemies"))
            {
                while (std::getline(mapFileStream, line))
                {
                    sstr.clear();
                    sstr.str(line);
                    if (!(sstr >> xMin >> xMax >> yMin >> yMax >> xMinMoveLimit >> xMaxMoveLimit))
                    {
                        sstr.clear();
                        sstr >> str;
                        break;
                    }
                    enemies.element.push_back(EnemyRectangle(xMin, xMax, yMin, yMax,xMinMoveLimit,xMaxMoveLimit));
                }
            }
            else if (!str.compare("finish"))
            {
                while (std::getline(mapFileStream, line))
                {
                    sstr.clear();
                    sstr.str(line);
                    if (!(sstr >> xMin >> xMax >> yMin >> yMax))
                    {
                        sstr.clear();
                        sstr >> str;
                        break;
                    }
                    finish.element = Rectangle(xMin, xMax, yMin, yMax);
                }
            }
            else if (!str.compare("mapHeight"))
            {
                while (std::getline(mapFileStream, line))
                {
                    float temp;
                    sstr.clear();
                    sstr.str(line);
                    if (!(sstr >> temp))
                    {
                        sstr.clear();
                        sstr >> str;
                        break;
                    }
                    mapHeight = temp;
                }
            }
            else if (!str.compare("mapWidth"))
            {
                while (std::getline(mapFileStream, line))
                {
                    float temp;
                    sstr.clear();
                    sstr.str(line);
                    if (!(sstr >> temp))
                    {
                        sstr.clear();
                        sstr >> str;
                        break;
                    }
                    mapWidth = temp;
                }
            }
        }
    }
    else
    {
        fprintf(stderr,"Impossible to open %s. Are you in the right directory?\n", mapPath);
    }
}

void OpenGLManager::loadPlatforms()
{
    for (std::list<Rectangle>::iterator it = platforms.element.begin(); it != platforms.element.end(); it++)
    {
        std::list<float> toAdd = it->getVerticesData();
        platforms.vertsData.insert(platforms.vertsData.end(), toAdd.begin(), toAdd.end());
    }

    glGenBuffers(1, &platforms.vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, platforms.vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, platforms.vertsData.size() * sizeof(float), &platforms.vertsData.front(), GL_DYNAMIC_DRAW);
    glGenVertexArrays(1, &platforms.vaoId);
    glBindVertexArray(platforms.vaoId);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

void OpenGLManager::loadAgent()
{
    std::list<float> toAdd = agent.element.getVerticesData();
    agent.vertsData.insert(agent.vertsData.end(), toAdd.begin(), toAdd.end());

    glGenBuffers(1, &agent.vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, agent.vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, agent.vertsData.size() * sizeof(float), &agent.vertsData.front(), GL_DYNAMIC_DRAW);
    glGenVertexArrays(1, &agent.vaoId);
    glBindVertexArray(agent.vaoId);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

void OpenGLManager::loadFinish()
{
    std::list<float> toAdd = finish.element.getVerticesData();
    finish.vertsData.insert(finish.vertsData.end(), toAdd.begin(), toAdd.end());

    glGenBuffers(1, &finish.vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, finish.vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, finish.vertsData.size() * sizeof(float), &finish.vertsData.front(), GL_DYNAMIC_DRAW);
    glGenVertexArrays(1, &finish.vaoId);
    glBindVertexArray(finish.vaoId);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

void OpenGLManager::loadCollectables()
{
    pointsNum = collectables.element.size();

    if (!collectables.element.empty())
    {
        for (std::list<Rectangle>::iterator it = collectables.element.begin(); it != collectables.element.end(); it++)
        {
            std::list<float> toAdd = it->getVerticesData();
            collectables.vertsData.insert(collectables.vertsData.end(), toAdd.begin(), toAdd.end());
        }

        glGenBuffers(1, &collectables.vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, collectables.vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, collectables.vertsData.size() * sizeof(float), &collectables.vertsData.front(), GL_DYNAMIC_DRAW);
        glGenVertexArrays(1, &collectables.vaoId);
        glBindVertexArray(collectables.vaoId);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    }
}

void OpenGLManager::loadEnemies()
{
    enemyNum = enemies.element.size();

    if (!enemies.element.empty())
    {
        for (std::list<EnemyRectangle>::iterator it = enemies.element.begin(); it != enemies.element.end(); it++)
        {
            std::list<float> toAdd = it->getVerticesData();
            enemies.vertsData.insert(enemies.vertsData.end(), toAdd.begin(), toAdd.end());
        }

        glGenBuffers(1, &enemies.vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, enemies.vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, enemies.vertsData.size() * sizeof(float), &enemies.vertsData.front(), GL_DYNAMIC_DRAW);
        glGenVertexArrays(1, &enemies.vaoId);
        glBindVertexArray(enemies.vaoId);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    }
}

GLuint OpenGLManager::loadShaders(const char* vertex_file_path, const char* fragment_file_path)
{
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);


    // Vertex shader
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);

    if (VertexShaderStream.is_open())
    {
        std::stringstream sstr;
        sstr << VertexShaderStream.rdbuf();
        VertexShaderCode = sstr.str();
        VertexShaderStream.close();
    }
    else
    {
        fprintf(stderr,"Impossible to open %s. Are you in the right directory?\n", vertex_file_path);
        getchar();
        return 0;
    }

    // Fragment shader
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);

    if (FragmentShaderStream.is_open())
    {
        std::stringstream sstr;
        sstr << FragmentShaderStream.rdbuf();
        FragmentShaderCode = sstr.str();
        FragmentShaderStream.close();
    }
    else
    {
        fprintf(stderr,"Impossible to open %s. Are you in the right directory?\n", fragment_file_path);
        getchar();
        return 0;
    }

    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Vertex shader compiling
    //printf("Compiling shader: %s\n", vertex_file_path);
    char const* VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
    glCompileShader(VertexShaderID);

    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);

    if (InfoLogLength > 0)
    {
        std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
        fprintf(stderr,"%s\n", &VertexShaderErrorMessage[0]);
    }

    //Fragment shader compiling
    //printf("Compiling shader: %s\n", fragment_file_path);
    char const* FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
    glCompileShader(FragmentShaderID);

    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);

    if (InfoLogLength > 0)
    {
        std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
        fprintf(stderr,"%s\n", &FragmentShaderErrorMessage[0]);
    }

    // Linking
    //printf("Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);

    if (InfoLogLength > 0)
    {
        std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        fprintf(stderr,"%s\n", &ProgramErrorMessage[0]);
    }

    glDetachShader(ProgramID, VertexShaderID);
    glDetachShader(ProgramID, FragmentShaderID);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    return ProgramID;
}

#pragma endregion initialization


bool OpenGLManager::stillRunning()
{
    return (!glfwWindowShouldClose(window) && (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS));
}

//void OpenGLManager::updateForLoop(bool* rightButtonPressed, bool* leftButtonPressed, bool* spacePressed,Rectangle* agent)
//{
//    *agent = this->agent.element;
//
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//    /*float currentTime = glfwGetTime();
//    float deltaTime = currentTime - *lastTime;
//    *lastTime = currentTime;*/
//
//
//    Model = getModelMatrix(rightButtonPressed,leftButtonPressed,spacePressed);
//    glm::mat4 MVPmatrix = Projection * View * Model;
//    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVPmatrix[0][0]);
//
//
//
//    glUseProgram(programID);
//
//    if (windowVisible)
//    {
//        draw();
//        glfwSwapBuffers(window);
//
//        glfwPollEvents();
//    }
//
//    
//}

void OpenGLManager::updateForLoop(bool* rightButtonPressed, bool* leftButtonPressed, bool* spacePressed,
    std::string logPosFilename, int stepsCounter)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /*float currentTime = glfwGetTime();
    float deltaTime = currentTime - *lastTime;
    *lastTime = currentTime;*/


    Model = getModelMatrix(rightButtonPressed, leftButtonPressed, spacePressed,logPosFilename,stepsCounter);
    glm::mat4 MVPmatrix = Projection * View * Model;
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVPmatrix[0][0]);

    Record record = agentManager->getRecord(stepsCounter, getDistanceX(), getDistanceY());
    
    
    /*std::ofstream log(logPosFilename, std::ios_base::app);
    log << "points\t" << record.getPoints() << "\n"
        << "health\t" << record.getHealth() << "\n"
        << "enemies\t" << record.getEnemies() << "\n"
        << "finished\t" << record.getFinished() << "\n"
        << "steps\t" << record.getSteps() << "\n"
        << "distance x\t" << record.getDistanceX() << "\n"
        << "distance y\t" << record.getDistanceY() << "\n\n";
    log.close();*/

    glUseProgram(programID);

    if (windowVisible)
    {
        draw();
        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    
}


//Rectangle* OpenGLManager::getNearestPoint(std::vector<std::list<Rectangle>::iterator> visiblePoints)
//{
//    if (visiblePoints.size() == 0)
//    {
//        return NULL;
//    }
//    else
//    {
//        float* agentXVal = new float();
//        float* agentYVal = new float();
//
//        float* pointXVal = new float();
//        float* pointYVal = new float();
//
//        float* pointXTemp = new float();
//        float* pointYTemp = new float();
//
//
//        agent.element.getCenter(agentXVal, agentYVal);
//        std::list<Rectangle>::iterator nearestPoint = visiblePoints[0];
//        (*visiblePoints[0]).getCenter(pointXVal, pointYVal);
//        float distance;
//        distance = sqrtf(pow(agentXVal - pointXVal, 2) + pow(agentYVal - pointYVal, 2));
//        //sprawdziæ czy nie ma czegoœ na drodze do przejœcia
//        for (int i = 1; i < visiblePoints.size(); i++)
//        {
//            (*visiblePoints[i]).getCenter(pointXTemp, pointYTemp);
//            float distanceTemp = sqrtf(pow(agentXVal - pointXTemp, 2) + pow(agentYVal - pointYTemp, 2));
//            if (distanceTemp < distance)
//            {
//                nearestPoint = visiblePoints[i];
//                distance = distanceTemp;
//            }
//        }
//
//
//        delete agentXVal;
//        delete agentYVal;
//        delete pointXVal;
//        delete pointYVal;
//        delete pointXTemp;
//        delete pointYTemp;
//
//        return &(*nearestPoint);
//    }
//}

//EnemyRectangle* OpenGLManager::getNearestEnemy(std::vector<std::list<EnemyRectangle>::iterator> visibleEnemies, std::vector<std::list<Rectangle>::iterator> visiblePlatforms)
//{
//    if (visibleEnemies.size() == 0)
//    {
//        return NULL;
//    }
//    else
//    {
//        float* agentXVal = new float();
//        float* agentYVal = new float();
//
//        float* enemyXVal = new float();
//        float* enemyYVal = new float();
//
//        float* enemyXTemp = new float();
//        float* enemyYTemp = new float();
//
//
//        agent.element.getCenter(agentXVal, agentYVal);
//        std::list<EnemyRectangle>::iterator nearestEnemy = visibleEnemies[0];
//        (*visibleEnemies[0]).getCenter(enemyXVal, enemyYVal);
//        float distance;
//        distance = sqrtf(pow(agentXVal-enemyXVal,2)+pow(agentYVal-enemyYVal,2));
//
//        if (*enemyYVal > *agentYVal)
//        {
//
//            for (int i = 0; i < visiblePlatforms.size(); i++)
//            {
//                float platformYCenter = (*visiblePlatforms[i]).getYCenter();
//                if ((platformYCenter < (*enemyYVal)) && (platformYCenter > (*agentYVal)))
//                {
//                    if ((*visiblePlatforms[i]).getXMin() < (*enemyXVal) < (*visiblePlatforms[i]).getXMax())
//                    {
//                        //jest gdzieœ wy¿ej na tej konkretnej platformie wróg
//                    }
//                }
//            }
//        }
//
//
//        //sprawdziæ czy nie ma czegoœ na drodze do przejœcia
//        for (int i = 1; i < visibleEnemies.size(); i++)
//        {
//            (*visibleEnemies[i]).getCenter(enemyXTemp, enemyYTemp);
//            float distanceTemp = sqrtf(pow(agentXVal - enemyXTemp, 2) + pow(agentYVal - enemyYTemp, 2));
//            if (distanceTemp < distance)
//            {
//                nearestEnemy = visibleEnemies[i];
//                distance = distanceTemp;
//            }
//        }
//
//
//        delete agentXVal;
//        delete agentYVal;
//        delete enemyXVal;
//        delete enemyYVal;
//        delete enemyXTemp;
//        delete enemyYTemp;
//
//        return &(*nearestEnemy);
//    }
//}

//Input OpenGLManager::getCurrentInputs(int healthLimit, EnemyRectangle* nearestEnemy, Rectangle* nearestPoint, std::vector<std::list<Rectangle>::iterator> visiblePlatformsIter)
//{
//    bool overHealthLimit;
//    bool enemyVisible;
//    bool pointVisible;
//    bool platformVisible;
//
//    if (agentManager->getHealth() <= healthLimit)
//    {
//        overHealthLimit = true;
//    }
//    else
//    {
//        overHealthLimit = false;
//    }
//
//    float* xAgentCenter = new float();
//    float* yAgentCenter = new float();
//    float* collisionValue = new float();
//
//    agent.element.getCenter(xAgentCenter, yAgentCenter);
//
//    //glm::mat4 MVPmatrix = Projection * View * Model;
//   // MVPmatrix = glm::inverse(MVPmatrix);
//
//    
//
//    Rectangle windowRectangle = Rectangle(*xAgentCenter - worldSizeHalfWidth, *xAgentCenter + worldSizeHalfWidth, *yAgentCenter - worldSizeHalfHeight, *yAgentCenter + worldSizeHalfHeight);
//
//    std::vector<std::list<EnemyRectangle>::iterator> visibleEnemies;
//    for (std::list<EnemyRectangle>::iterator it = enemies.element.begin(); it != enemies.element.end(); it++)
//    {
//        ColliderWall coll = (*it).collision(windowRectangle, collisionValue);
//        if (coll != None)
//        {
//            visibleEnemies.push_back(it);
//        }
//        
//    }
//    if (visibleEnemies.size() > 0)
//    {
//        enemyVisible = true;
//        nearestEnemy = getNearestEnemy(visibleEnemies,visiblePlatformsIter);
//    }
//    else
//    {
//        enemyVisible = false;
//        nearestEnemy = NULL;
//    }
//
//    std::vector<std::list<Rectangle>::iterator> visibleCollectables;
//    for (std::list<Rectangle>::iterator it = collectables.element.begin(); it != collectables.element.end(); it++)
//    {
//        ColliderWall coll = (*it).collision(windowRectangle, collisionValue);
//        if (coll != None)
//        {
//            visibleCollectables.push_back(it);
//        }
//
//    }
//    if (visibleCollectables.size() > 0)
//    {
//        pointVisible = true;
//        nearestPoint = getNearestPoint(visibleCollectables);
//    }
//    else
//    {
//        pointVisible = false;
//        nearestPoint = NULL;
//    }
//
//    std::vector<std::list<Rectangle>::iterator> visiblePlatforms;
//    for (std::list<Rectangle>::iterator it = platforms.element.begin(); it != platforms.element.end(); it++)
//    {
//        ColliderWall coll = (*it).collision(windowRectangle, collisionValue);
//        if (coll != None)
//        {
//            visiblePlatforms.push_back(it);
//        }
//
//    }
//    if (visiblePlatforms.size() > 0)
//    {
//        platformVisible = true;
//    }
//    else
//    {
//        platformVisible = false;
//    }
//    visiblePlatformsIter = visiblePlatforms;
//
//    delete xAgentCenter;
//    delete yAgentCenter;
//    delete collisionValue;
//    return Input(overHealthLimit,enemyVisible,pointVisible,platformVisible);
//}

SimpleInput OpenGLManager::getCurrentInputs()
{
    float endOfPlatformDetection = 1.0f;
    SimpleInput input;
    if (agentManager->getHealth() <= 50)
    {
        input.lowHealth = true;
    }

    //end of platform
    float* collisionValue = new float();
    for (std::list<Rectangle>::iterator it = platforms.element.begin(); it != platforms.element.end(); it++)
    {
        ColliderWall coll = (*it).collision(agent.element, collisionValue);
        if (coll == Bottom)
        {
            if ((*it).getXMin() + endOfPlatformDetection >= agent.element.getXMin())
            {
                input.endOfPlatformOnLeft = true;
            }
            if ((*it).getXMax() - endOfPlatformDetection <= agent.element.getXMax())
            {
                input.endOfPlatformOnRight = true;
            }
            break;
        }
    }

    //enemy on left/right
    Rectangle detectionRectangle = Rectangle(agent.element.getXCenter() - worldSizeHalfWidth, agent.element.getXCenter() + worldSizeHalfWidth,
        agent.element.getYMin(),agent.element.getYMax());
    Rectangle floatingDetectionRectangle = Rectangle(agent.element.getXCenter() - worldSizeHalfWidth / 3.0, agent.element.getXCenter() + worldSizeHalfWidth / 3.0,
        agent.element.getYCenter() - worldSizeHalfHeight, agent.element.getYCenter() + worldSizeHalfHeight);

    std::vector<std::list<EnemyRectangle>::iterator> enemiesInRange;
    for (std::list<EnemyRectangle>::iterator it = enemies.element.begin(); it != enemies.element.end(); it++)
    {
        ColliderWall coll = (*it).collision(detectionRectangle, collisionValue);
        if (coll != None)
        {
            enemiesInRange.push_back(it);
        }
    }
    for (int i = 0; i < enemiesInRange.size(); i++)
    {
        if ((*enemiesInRange[i]).getXCenter() > agent.element.getXCenter())
        {
            input.enemyOnRight = true;
        }
        else if ((*enemiesInRange[i]).getXCenter() < agent.element.getXCenter())
        {
            input.enemyOnLeft = true;
        }
    }

    //finish on left/right
    if (finish.element.collision(detectionRectangle, collisionValue))
    {
        if (finish.element.getXCenter() > agent.element.getXCenter())
        {
            input.enemyOnRight = true;
        }
        else if (finish.element.getXCenter() < agent.element.getXCenter())
        {
            input.enemyOnLeft = true;
        }
    }

    //platform above on right/left
    std::vector<std::list<Rectangle>::iterator> platformsInRange;
    for (std::list<Rectangle>::iterator it = platforms.element.begin(); it != platforms.element.end(); it++)
    {
        ColliderWall coll = (*it).collision(floatingDetectionRectangle, collisionValue);
        if (coll != None)
        {
            platformsInRange.push_back(it);
        }
    }
    for (int i = 0; i < platformsInRange.size(); i++)
    {
        if ((*platformsInRange[i]).getXCenter() > agent.element.getXCenter())
        {
            input.platformAboveOnRight = true;
        }
        else if ((*platformsInRange[i]).getXCenter() < agent.element.getXCenter())
        {
            input.platformAboveOnLeft = true;
        }
    }

    //point on left/right
    std::vector<std::list<Rectangle>::iterator> collectablesInRange;
    for (std::list<Rectangle>::iterator it = collectables.element.begin(); it != collectables.element.end(); it++)
    {
        ColliderWall coll = (*it).collision(floatingDetectionRectangle, collisionValue);
        if (coll != None)
        {
            collectablesInRange.push_back(it);
        }
    }
    for (int i = 0; i < collectablesInRange.size(); i++)
    {
        if ((*collectablesInRange[i]).getXCenter() > agent.element.getXCenter())
        {
            input.pointOnRight = true;
        }
        else if ((*collectablesInRange[i]).getXCenter() < agent.element.getXCenter())
        {
            input.pointOnLeft = true;
        }
    }

    delete collisionValue;
    return input;
}

int OpenGLManager::getPointsNum()
{
    return pointsNum;
}

int OpenGLManager::getEnemyNum()
{
    return enemyNum;
}

float OpenGLManager::getDistanceX()
{
    if (agent.element.getXMax() < finish.element.getXMin())
    {
        return finish.element.getXMin() - agent.element.getXMax();
    }
    else if (agent.element.getXMin() > finish.element.getXMax())
    {
        return agent.element.getXMin() - finish.element.getXMax();
    }
    else
    {
        return 0.0f;
    }
}

float OpenGLManager::getDistanceY()
{
    if (agent.element.getYMax() < finish.element.getYMin())
    {
        return finish.element.getYMin() - agent.element.getYMax();
    }
    else if (agent.element.getYMin() > finish.element.getYMax())
    {
        return agent.element.getYMin() - finish.element.getYMax();
    }
    else
    {
        return 0.0f;
    }
}

float OpenGLManager::getMapHeight()
{
    return mapHeight;
}

float OpenGLManager::getMapWidth()
{
    return mapWidth;
}



void OpenGLManager::draw()
{
    //platformy
    glUniform3f(colorUniform, 1.0f, 0.0f, 0.0f);
    glBindVertexArray(platforms.vaoId);
    for (int i = 0; i < platforms.element.size(); i++)
    {
        glDrawArrays(GL_TRIANGLE_STRIP, i * 4, 4);
    }

    //agent
    glUniform3f(colorUniform, 0.0f, 0.0f, 1.0f);
    glBindVertexArray(agent.vaoId);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    //do zbierania
    glUniform3f(colorUniform, 1.0f, 1.0f, 0.0f);
    glBindVertexArray(collectables.vaoId);
    for (int i = 0; i < collectables.element.size(); i++)
    {
        glDrawArrays(GL_TRIANGLE_STRIP, i * 4, 4);
    }

    //przeciwnicy
    glUniform3f(colorUniform, 0.0f, 1.0f, 0.0f);
    glBindVertexArray(enemies.vaoId);
    for (int i = 0; i < enemies.element.size(); i++)
    {
        glDrawArrays(GL_TRIANGLE_STRIP, i * 4, 4);
    }

    //koniec
    glUniform3f(colorUniform, 0.0f, 1.0f, 1.0f);
    glBindVertexArray(finish.vaoId);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}


void OpenGLManager::updateEnemies()
{
    enemies.vertsData.clear();
    for (std::list<EnemyRectangle>::iterator it = enemies.element.begin(); it != enemies.element.end(); it++)
    {
        //(*it).moveX(deltaTime*enemySpeed); //change
        (*it).moveX(basicMoveChangeEnemy);

        std::list<float> toAdd = it->getVerticesData();
        enemies.vertsData.insert(enemies.vertsData.end(), toAdd.begin(), toAdd.end());
    }

    glBindBuffer(GL_ARRAY_BUFFER, enemies.vertexBuffer);
    if (enemies.vertsData.size() != 0)
    {
        glBufferSubData(GL_ARRAY_BUFFER, 0, enemies.vertsData.size() * sizeof(float), &enemies.vertsData.front());
    }
    else
    {
        glBufferSubData(GL_ARRAY_BUFFER, 0, 0, NULL);
    }
}

void OpenGLManager::updateCollectables()
{
    collectables.vertsData.clear();
    for (std::list<Rectangle>::iterator it = collectables.element.begin(); it != collectables.element.end(); it++)
    {
        std::list<float> toAdd = it->getVerticesData();
        collectables.vertsData.insert(collectables.vertsData.end(), toAdd.begin(), toAdd.end());
    }

    glBindBuffer(GL_ARRAY_BUFFER, collectables.vertexBuffer);
    if (collectables.vertsData.size() != 0)
    {
        glBufferSubData(GL_ARRAY_BUFFER, 0, collectables.vertsData.size() * sizeof(float), &collectables.vertsData.front());
    }
    else
    {
        glBufferSubData(GL_ARRAY_BUFFER, 0, 0, NULL);
    }
}

void OpenGLManager::updateAgent()
{
    float xChange = agentManager->getXChange();
    float yChange = agentManager->getYChange();
    if (xChange != 0.0f)
    {
        for (int i = 0; i < agent.vertsData.size(); i += 3)
        {
            agent.vertsData[i] += xChange; //TODO
        }
        agent.element.moveX(xChange);
    }
    if (yChange != 0.0f)
    {
        for (int i = 0; i < agent.vertsData.size(); i += 3)
        {
            agent.vertsData[i+1] += yChange; //TODO
        }
        agent.element.moveY(yChange);
    }

    if (xChange != 0.0f || yChange != 0.0f)
    {
        glBindBuffer(GL_ARRAY_BUFFER, agent.vertexBuffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0, agent.vertsData.size() * sizeof(float), &agent.vertsData.front());
    }
}

glm::mat4 OpenGLManager::getModelMatrix(bool* rightButtonPressed, bool* leftButtonPressed, bool* spacePressed)
{
    //kolizje z platformami
    bool left = false;
    bool right = false;
    bool top = false;
    bool bottom = false;

    float* collisionValue = new float(0);

    

    for (std::list<Rectangle>::iterator it = platforms.element.begin(); it != platforms.element.end(); it++)
    {
        ColliderWall coll = (*it).collision(agent.element, collisionValue);
        switch (coll)
        {
        case None:
            break;
        case Left:
            agentManager->addToXChange(*collisionValue);
            globalXChange -= *collisionValue;
            left = true;
            break;
        case Right:
            agentManager->addToXChange(-*collisionValue);
            globalXChange -= -*collisionValue;
            right = true;
            break;
        case Top:
            agentManager->addToYChange(-*collisionValue);
            globalYChange -= -*collisionValue;
            top = true;
            break;
        case Bottom:
            agentManager->addToYChange(*collisionValue);
            globalYChange -= *collisionValue;
            bottom = true;
            break;
        }

    }

    //kolizje z punktami
    bool collectablesChanged = false;
    std::vector<std::list<Rectangle>::iterator> toDeleteCollectables;
    for (std::list<Rectangle>::iterator it = collectables.element.begin(); it != collectables.element.end(); it++)
    {
        ColliderWall coll = (*it).collision(agent.element, collisionValue);
        if (coll != None)
        {
            toDeleteCollectables.push_back(it);
            agentManager->addPoints(1);
            collectablesChanged = true;
            //printf("Points: %d \n", agentManager->getPoints());
        }
    }
    for (int i = 0; i < toDeleteCollectables.size(); i++)
    {
        collectables.element.erase(toDeleteCollectables[i]);
    }

    if (collectablesChanged)
    {
        updateCollectables();
    }

    agentManager->updateImmunity();

    //kolizje z wrogami
    std::vector<std::list<EnemyRectangle>::iterator> toDeleteEnemies;
    for (std::list<EnemyRectangle>::iterator it = enemies.element.begin(); it != enemies.element.end(); it++)
    {
        ColliderWall coll = (*it).collision(agent.element, collisionValue);
        if (coll == Bottom && !(*it).getCollided())
        {
            toDeleteEnemies.push_back(it);
            //agentManager->addPoints(enemyDeathPoints);
            agentManager->enemyDefeat();
            //printf("Points: %d \n", agentManager->getPoints());
        }
        else if ((coll == Left || coll == Right) && !agentManager->getIsImmune())
        {
            agentManager->enemyAttack(enemyDeathPoints);

            (*it).setCollided(true);

        }
        else if (coll == None)
        {
            (*it).setCollided(false);
        }
    }
    for (int i = 0; i < toDeleteEnemies.size(); i++)
    {
        enemies.element.erase(toDeleteEnemies[i]);
    }

    //kolizje z wyjsciem i warunki konca
    ColliderWall coll = finish.element.collision(agent.element, collisionValue);
    if (coll != None)
    {
        agentManager->setFinished(true);
    }


    //przemieszczanie agenta
    if (*rightButtonPressed && !right)
        //if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && !right) //test rêczny
    {
        agentManager->addToXChange(basicMoveChange);
        globalXChange -= basicMoveChange;
        //globalXChange -= agentManager->addToXChangeTime(deltaTime);
    }

    if (*leftButtonPressed && !left)
        //if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && !left) //test rêczny
    {
        agentManager->addToXChange(-basicMoveChange);
        globalXChange -= -basicMoveChange;
        //globalXChange -= agentManager->addToXChangeTime(-deltaTime);
    }

    if (!bottom)
    {
        agentManager->addToYChange(-basicMoveChange);
        globalYChange -= -basicMoveChange;
        //agentManager->addToYChange( -gravity * deltaTime);
        //globalYChange -= -gravity * deltaTime;
    }

    //*spacePressed = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS; //test rêczny
    globalYChange -= agentManager->handleJumping(basicMoveChange, top, bottom);
    globalYChange -= agentManager->startJumping(basicMoveChange, top, bottom, spacePressed);
    /*globalYChange -= agentManager->handleJumping(deltaTime, top, bottom);
    globalYChange -= agentManager->startJumping(deltaTime, top, bottom, spacePressed);*/



    updateAgent();
    updateEnemies();

    agentManager->cleanXYChange();

    

    return glm::translate(glm::mat4(1.0f), glm::vec3(globalXChange, globalYChange, 0.0f));
}

glm::mat4 OpenGLManager::getModelMatrix(bool* rightButtonPressed, bool* leftButtonPressed, bool* spacePressed, 
    std::string logPositionFile, int step)
{
    //kolizje z platformami
    bool left = false;
    bool right = false;
    bool top = false;
    bool bottom = false;

    float* collisionValue = new float(0);

    //std::ofstream logPos(logPositionFile, std::ios_base::app);

    //logPos << "Step " << std::to_string(step) << "\n";

    for (std::list<Rectangle>::iterator it = platforms.element.begin(); it != platforms.element.end(); it++)
    {
        ColliderWall coll = (*it).collision(agent.element, collisionValue);
        switch (coll)
        {
        case None:
            break;
        case Left:
            agentManager->addToXChange(*collisionValue);
            globalXChange -= *collisionValue;
            left = true;
            //logPos << "LeftColl\t" << std::to_string(*collisionValue) << "\n";
            break;
        case Right:
            agentManager->addToXChange(-*collisionValue);
            globalXChange -= -*collisionValue;
            right = true;
            //logPos << "RightColl\t" << std::to_string(*collisionValue) << "\n";
            break;
        case Top:
            agentManager->addToYChange(-*collisionValue);
            globalYChange -= -*collisionValue;
            top = true;
            //logPos << "TopColl\t" << std::to_string(*collisionValue) << "\n";
            break;
        case Bottom:
            agentManager->addToYChange(*collisionValue);
            globalYChange -= *collisionValue;
            bottom = true;
            //logPos << "BottomColl\t" << std::to_string(*collisionValue) << "\n";
            break;
        }

    }

    //kolizje z punktami
    bool collectablesChanged = false;
    std::vector<std::list<Rectangle>::iterator> toDeleteCollectables;
    for (std::list<Rectangle>::iterator it = collectables.element.begin(); it != collectables.element.end(); it++)
    {
        ColliderWall coll = (*it).collision(agent.element, collisionValue);
        if (coll != None)
        {
            toDeleteCollectables.push_back(it);
            agentManager->addPoints(1);
            collectablesChanged = true;
            //printf("Points: %d \n", agentManager->getPoints());
        }
    }
    for (int i = 0; i < toDeleteCollectables.size(); i++)
    {
        collectables.element.erase(toDeleteCollectables[i]);
    }

    if (collectablesChanged)
    {
        updateCollectables();
    }
    
    agentManager->updateImmunity();

    //kolizje z wrogami
    std::vector<std::list<EnemyRectangle>::iterator> toDeleteEnemies;
    for (std::list<EnemyRectangle>::iterator it = enemies.element.begin(); it != enemies.element.end(); it++)
    {
        ColliderWall coll = (*it).collision(agent.element,collisionValue);
        if (coll == Bottom && !(*it).getCollided())
        {
            //logPos << "EnemyBottomColl\t" << std::to_string(*collisionValue) << "\n";
            toDeleteEnemies.push_back(it);
            //agentManager->addPoints(enemyDeathPoints);
            agentManager->enemyDefeat();
            //printf("Points: %d \n", agentManager->getPoints());
        }
        else if ((coll == Left || coll == Right) && !agentManager->getIsImmune())
        {
            if (coll == Left)
            {
                //logPos << "EnemyLeftColl\t" << std::to_string(*collisionValue) << "\n";
            }
            else
            {
                //logPos << "EnemyRightColl\t" << std::to_string(*collisionValue) << "\n";
            }

            agentManager->enemyAttack(enemyDeathPoints);
            
            (*it).setCollided(true);
            
        }
        else if ((coll == Left || coll == Right))
        {
            if (coll == Left)
            {
                //logPos << "Immune! EnemyLeftColl\t" << std::to_string(*collisionValue) << "\n";
            }
            else
            {
                //logPos << "Immune! EnemyRightColl\t" << std::to_string(*collisionValue) << "\n";
            }
        }
        else if (coll == None)
        {
            (*it).setCollided(false);
        }
        else
        {
            if (coll == Top)
            {
                //logPos << "EnemyTopColl\t" << std::to_string(*collisionValue) << "\n";
            }
            else if (coll == Inside)
            {
                //logPos << "EnemyInsideColl\t" << std::to_string(*collisionValue) << "\n";
            }
        }
    }
    for (int i = 0; i < toDeleteEnemies.size(); i++)
    {
        enemies.element.erase(toDeleteEnemies[i]);
    }

    //kolizje z wyjsciem i warunki konca
    ColliderWall coll = finish.element.collision(agent.element,collisionValue);
    if (coll != None)
    {
        agentManager->setFinished(true);
    }


    //przemieszczanie agenta
    if (*rightButtonPressed && !right)
    //if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && !right) //test rêczny
    {
        agentManager->addToXChange(basicMoveChange);
        globalXChange -= basicMoveChange;
        //globalXChange -= agentManager->addToXChangeTime(deltaTime);
    }

    if (*leftButtonPressed && !left)
    //if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && !left) //test rêczny
    {
        agentManager->addToXChange(-basicMoveChange);
        globalXChange -= -basicMoveChange;
        //globalXChange -= agentManager->addToXChangeTime(-deltaTime);
    }

    if (!bottom)
    {
        agentManager->addToYChange(-basicMoveChange);
        globalYChange -=  -basicMoveChange;
        //agentManager->addToYChange( -gravity * deltaTime);
        //globalYChange -= -gravity * deltaTime;
    }

    //*spacePressed = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS; //test rêczny
    globalYChange -= agentManager->handleJumping(basicMoveChange,top,bottom);
    globalYChange -= agentManager->startJumping(basicMoveChange, top, bottom,spacePressed);
    /*globalYChange -= agentManager->handleJumping(deltaTime, top, bottom);
    globalYChange -= agentManager->startJumping(deltaTime, top, bottom, spacePressed);*/
    
   

    updateAgent();
    updateEnemies();
    
    agentManager->cleanXYChange();

    //logPos << "Position\t" << agent.element.getXCenter() << "\t" << agent.element.getYCenter() << "\n";

    /*for (std::list<EnemyRectangle>::iterator it = enemies.element.begin(); it != enemies.element.end(); it++)
    {
        logPos << "EnemyPosition\t" << (*it).getXCenter() << "\t" << (*it).getYCenter() << "\n";
    }*/

    //logPos << "\n";

    //logPos.close();
    delete collisionValue;

    return glm::translate(glm::mat4(1.0f), glm::vec3(globalXChange, globalYChange, 0.0f));
}




