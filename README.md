🛠️ Step 1: The Blueprint (include/common.h and include/config.h)

Before you write a single line of executable code, you need to define what your data looks like. If you don't do this first, you’ll end up constantly rewriting functions because your structs changed.

    What to put in common.h: Define your coordinate structures, the game state structure (score, remaining aliens, rocket counts), and the structural definitions for an Alien and a Rocket.

    What to put in config.h: Hardcode some initial numbers for your difficulty settings (e.g., #define EASY_RELOAD_TIME 1000000).

🚀 Step 2: The Core Loop (src/main.c)

Don't try to spawn 20 threads immediately. Start with a clean main.c that initializes your game state and sets up a standard, predictable game loop.

    Write the menu selection for difficulty.

    Initialize your global data structures using the settings from config.h.

    Create a temporary while(!game_over) loop just to make sure your game can start, update state, and exit cleanly on a basic condition (like hitting a keyboard key).

📺 Step 3: The Visuals (src/render.c and include/render.h)

It is incredibly difficult to debug a terminal game if you can't see what the program thinks is happening.

    Implement a simple draw_game() function.

    If you are using ncurses, initialize the screen here (initscr(), cbreak(), noecho()).

    If you are using standard ANSI escape codes, write a function that clears the terminal and draws a border, the battery status, and a mock alien at a fixed coordinate.

    Call this draw function inside your main.c loop.

⌨️ Step 4: Your Very First Thread (src/input.c and include/input.h)

This is where you introduce pthreads. Keyboard input in C is normally blocking (the program stops and waits for you to hit a key). You want your game to keep running even if the player is sitting still.

    Write an input_handler function designed to run as a thread.

    Inside it, create a loop that listens for keys (e.g., Left/Right arrows to change the battery angle, Spacebar to trigger a "shoot" event).

    In main.c, initialize your very first mutex (pthread_mutex_init) and spawn this thread using pthread_create.

    Milestone: You should now be able to see your game rendering smoothly while you interactively change the anti-aircraft gun's angle in real-time.

👾 Next Steps From There

Once you have your input thread safely talking to your rendering loop without crashing, you can move on to the entity logic in src/entities.c:

    Implement the Reloader Thread (it's the easiest background worker to write; it just ticks up a counter every few seconds).

    Implement a single Alien Thread to watch it descend.

    Implement Projectile Threads spawned by your spacebar input.
