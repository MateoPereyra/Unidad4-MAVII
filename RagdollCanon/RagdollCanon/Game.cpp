#include "Game.h"
#include "Ragdoll.h"
#include "Box2DHelper.h"

std::vector<Ragdoll*> ragdolls;

// Constructor de la clase Game
Game::Game(int ancho, int alto, std::string titulo)
{
	wnd = new RenderWindow(VideoMode(ancho, alto), titulo);// Inicializa la ventana de renderizado con el tama�o y t�tulo especificados
	wnd->setVisible(true);// Hace que la ventana sea visible
	fps = 60;// Establece el l�mite de frames por segundo
	wnd->setFramerateLimit(fps);
	frameTime = 1.0f / fps;// Calcula el tiempo de cada frame
	SetZoom();// Establece el zoom del juego
	InitPhysics();// Inicializa el mundo f�sico de Box2D
}

// Bucle principal del juego
void Game::Loop()
{
	// Bucle principal de juego, se ejecuta mientras la ventana est� abierta
	while (wnd->isOpen())
	{
		wnd->clear(clearColor);// Limpia la ventana con el color clearColor
		DoEvents();// Gestiona los eventos de la ventana
		CheckCollitions();// Comprueba colisiones
		CannonRotation(); //Actualizo el ca�on
		UpdatePhysics();// Actualiza el mundo f�sico
		DrawGame();// Dibuja el juego
		wnd->display();// Muestra la ventana
	}
}

// Actualiza el mundo f�sico
void Game::UpdatePhysics()
{
	phyWorld->Step(frameTime, 8, 8);// Realiza un paso de simulaci�n en el mundo f�sico
	phyWorld->ClearForces();// Limpia las fuerzas aplicadas en el mundo f�sico
	phyWorld->DebugDraw();// Dibuja el mundo f�sico en modo de depuraci�n
}

void Game::DrawGame(){
	// Dibujar el ca�on (cuerpo de control)
	sf::RectangleShape cannonShape(sf::Vector2f(25.0f, 10.0f));
	cannonShape.setFillColor(sf::Color::Green);
	cannonShape.setPosition(controlBody->GetPosition().x, controlBody->GetPosition().y);
	cannonShape.setOrigin(0, cannonShape.getSize().y / 2); // Origen en la base del ca��n
	cannonShape.setRotation(controlBody->GetAngle() * 180 / b2_pi); // Box2D usa radianes, SFML grados
	wnd->draw(cannonShape);

	// Dibujar las paredes
	sf::RectangleShape leftWallShape(sf::Vector2f(10, alto)); // Alto de la ventana
	leftWallShape.setFillColor(sf::Color::Black);
	leftWallShape.setPosition(100, 0); // X = 100 para que comience donde termina el suelo
	wnd->draw(leftWallShape);

	// Dibujar el suelo
	sf::RectangleShape groundShape(sf::Vector2f(500, 5));
	groundShape.setFillColor(sf::Color::Red);
	groundShape.setPosition(0, 95);
	wnd->draw(groundShape);

	//Dibujar el techo
	sf::RectangleShape ceilinglShape(sf::Vector2f(500, 5));
	ceilinglShape.setFillColor(sf::Color::Red);
	ceilinglShape.setPosition(0, 0);
	wnd->draw(ceilinglShape);


}
// Gestiona los eventos de la ventana
void Game::DoEvents()
{
	// Obtener posici�n del mouse en coordenadas del mundo
	Vector2i mousePixel = Mouse::getPosition(*wnd);
	Vector2f mouseWorld = wnd->mapPixelToCoords(mousePixel);

	// Obtener posici�n del ca��n en pixeles
	b2Vec2 cannonPosMeters = controlBody->GetPosition();
	Vector2f cannonPosPixels(cannonPosMeters.x, cannonPosMeters.y);

	// Calcular �ngulo hacia el mouse
	float dx = mouseWorld.x - cannonPosPixels.x;
	float dy = mouseWorld.y - cannonPosPixels.y;
	float angle = std::atan2(dy, dx); //Radianes

	Event evt;
	while (wnd->pollEvent(evt))
	{
		switch (evt.type)
		{
		case Event::Closed:// Cierra la ventana si se presiona el bot�n de cerrar
			wnd->close();
			break;
		case Event::MouseButtonPressed:
			if (evt.mouseButton.button == Mouse::Left) {
				// Posici�n inicial del ca��n
				b2Vec2 spawnPos = controlBody->GetWorldCenter() + b2Vec2(10.0f / angle, 10.0f / angle); // punta del ca��n

				// Crear ragdoll
				Ragdoll* ragdoll = new Ragdoll(phyWorld, spawnPos, angle);

				// Impulso en la direcci�n que apunta el ca��n
				float angle = controlBody->GetAngle();
				b2Vec2 dir(std::cos(angle), std::sin(angle));
			b2Vec2 impulse(dir.x * 400.0f, dir.y * 400.0f);

			ragdoll->ApplyImpulse(impulse);
				ragdolls.push_back(ragdoll);
			}
			break;
		}
	}
}

void Game::CheckCollitions()
{
	// Veremos mas adelante
}

// Definimos el area del mundo que veremos en nuestro juego
// Box2D tiene problemas para simular magnitudes muy grandes
// Configura el �rea visible del juego
void Game::SetZoom()
{
	// Define el �rea visible del juego
	View v(Vector2f(50.0f, 50.0f), Vector2f(100.0f, 100.0f));
	wnd->setView(v);
}

void Game::CannonRotation() {
	// Obtener posici�n del mouse en coordenadas del mundo
	Vector2i mousePixel = Mouse::getPosition(*wnd);
	Vector2f mouseWorld = wnd->mapPixelToCoords(mousePixel);

	// Obtener posici�n del ca��n en pixeles
	b2Vec2 cannonPosMeters = controlBody->GetPosition();
	Vector2f cannonPosPixels(cannonPosMeters.x, cannonPosMeters.y); 

	// Calcular �ngulo hacia el mouse
	float dx = mouseWorld.x - cannonPosPixels.x;
	float dy = mouseWorld.y - cannonPosPixels.y;
	float angle = std::atan2(dy, dx); //Radianes

	// Aplicar rotaci�n
	controlBody->SetTransform(controlBody->GetPosition(), angle); 
}

void Game::InitPhysics()
{
	// Inicializamos el mundo con la gravedad por defecto
	phyWorld = new b2World(b2Vec2(0.0f, 9.8f));

	// Creamos el renderer de debug y le seteamos las banderas para que dibuje TODO
	debugRender = new SFMLRenderer(wnd);
	debugRender->SetFlags(UINT32_MAX);
	phyWorld->SetDebugDraw(debugRender);

	// Creamos un piso y paredes
	b2Body* groundBody = Box2DHelper::CreateRectangularStaticBody(phyWorld, 100, 10);
	groundBody->SetTransform(b2Vec2(50.0f, 100.0f), 0.0f);

	b2Body* leftWallBody = Box2DHelper::CreateRectangularStaticBody(phyWorld, 10, 100);
	leftWallBody->SetTransform(b2Vec2(0.0f, 50.0f), 0.0f);

	b2Body* rightWallBody = Box2DHelper::CreateRectangularStaticBody(phyWorld, 10, 100);
	rightWallBody->SetTransform(b2Vec2(100.0f, 50.0f), 0.0f);

	// Creamos un techo
	b2Body* topWallBody = Box2DHelper::CreateRectangularStaticBody(phyWorld, 100, 10);
	topWallBody->SetTransform(b2Vec2(50.0f, 0.0f), 0.0f);

	// Creaci�n del ca��n
	controlBody = Box2DHelper::CreateRectangularKinematicBody(phyWorld, 25, 10);
	controlBody->SetTransform(b2Vec2(5.0f, 100.0f), 0.0f);

}



Game::~Game(void)
{ }


// /c/Users/mateo/source/repos/Unidad4-MAVII