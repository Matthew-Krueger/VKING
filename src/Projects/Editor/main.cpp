/*
 *         A high-performance, module-first game engine.
 *         Copyright (C) 2026 Matthew Krueger
 *
 *         This program is free software: you can redistribute it and/or modify
 *         it under the terms of the GNU General Public License as published by
 *         the Free Software Foundation, either version 3 of the License, or
 *         (at your option) any later version.
 *
 *         This program is distributed in the hope that it will be useful,
 *         but WITHOUT ANY WARRANTY; without even the implied warranty of
 *         MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *         GNU General Public License for more details.
 *
 *         You should have received a copy of the GNU General Public License
 *         along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <iostream>

import VKING.Log;

using Log = VKING::Log::Named<"Editor">;

int main() {

    VKING::Log::Init("LogFile.log");
    VKING::Log::setLevel(VKING::Log::Level::debug);

    Log::record().critical("Starting up");

    Log::record().debug("Hello, World!");

    Log::record().critical("Hello, World!");

    glm::vec3 f(1.0,2.0,1.0);

    Log::record().critical("Hello, World! Your name is {}", "Matthew");


    VKING::Log::Named<"Test">::record().info("Value: {}", 42);

    auto z = f * 3.0f;

    std::cout << z.x << " " << z.y << " " << z.z << std::endl;

    glm::vec3 a  =  z.xyy;

    std::cout << a.x << " " << a.y << " " << a.z << std::endl;

    std::cout << "Hello, World!" << std::endl;
    return 0;
}
