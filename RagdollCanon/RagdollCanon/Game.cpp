#include "Game.h"
#include "Box2DHelper.h"

// Constructor de la clase Game
Game::Game(int ancho, int alto, std::string titulo)
{
	wnd = new RenderWindow(VideoMode(ancho, alto), titulo);// Inicializa la ventana de renderizado con el tamaño y título especificados
	wnd->setVisible(true);// Hace que la ventana sea visible
	fps = 60;// Establece el límite de frames por segundo
	wnd->setFramerateLimit(fps);
	frameTime = 1.0f / fps;// Calcula el tiempo de cada frame
	SetZoom();// Establece el zoom del juego
	InitPhysics();// Inicializa el mundo físico de Box2D
}

// Bucle principal del juego
void Game::Loop()
{
	// Bucle principal de juego, se ejecuta mientras la ventana esté abierta
	while (wnd->isOpen())
	{
		wnd->clear(clearColor);// Limpia la ventana con el color clearColor
		DoEvents();// Gestiona los eventos de la ventana
		CheckCollitions();// Comprueba colisiones
		UpdatePhysics();// Actualiza el mundo físico
		DrawGame();// Dibuja el juego
		wnd->display();// Muestra la ventana
	}
}

// Actualiza el mundo físico
void Game::UpdatePhysics()
{
	phyWorld->Step(frameTime, 8, 8);// Realiza un paso de simulación en el mundo físico
	phyWorld->ClearForces();// Limpia las fuerzas aplicadas en el mundo físico
	phyWorld->DebugDraw();// Dibuja el mundo físico en modo de depuración
}

void Game::DrawGame()
{ }
// Gestiona los eventos de la ventana
void Game::DoEvents()
{
	Event evt;
	while (wnd->pollEvent(evt))
	{
		switch (evt.type)
		{
		case Event::Closed:// Cierra la ventana si se presiona el botón de cerrar
			wnd->close();
			break;
		case Event::MouseButtonPressed:// Crea un cuerpo triangular dinámico en la posición del clic del ratón
			b2Body* body = Box2DHelper::CreateTriangularDynamicBody(phyWorld, b2Vec2(0.0f, 0.0f), 10.0f, 1.0f, 4.0f, 0.1f);
			// Transforma las coordenadas del clic del ratón según la vista activa
			Vector2f pos = wnd->mapPixelToCoords(Vector2i(evt.mouseButton.x, evt.mouseButton.y));
			body->SetTransform(b2Vec2(pos.x, pos.y), 0.0f);
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
// Configura el área visible del juego
void Game::SetZoom()
{
	// Define el área visible del juego
	View v(Vector2f(50.0f, 50.0f), Vector2f(100.0f, 100.0f));
	wnd->setView(v);
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

	// Crea una esfera y una caja unidas por una articulación revoluta (Esta es la parte del RevoluteJoint)
	b2Body* EsferaRev = Box2DHelper::CreateCircularStaticBody(phyWorld, 5.0f);
	EsferaRev->SetTransform(b2Vec2(50.0f, 50.0f), 0.0f);
	b2Body* CajaRev = Box2DHelper::CreateRectangularDynamicBody(phyWorld, 5, 5, 1.0f, 1.0f, 1.0f);
	CajaRev->SetTransform(b2Vec2(50.0f, 70.0f), 0.0f);
	b2RevoluteJoint* revJoint = Box2DHelper::CreateRevoluteJoint(phyWorld, EsferaRev, EsferaRev->GetWorldCenter(),
		CajaRev, -b2_pi / 2.0f, b2_pi / 2.0f, 2.0f, 1000.0f, true, true);
}

Game::~Game(void)
{ }


