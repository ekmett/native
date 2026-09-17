// SPDX-FileType: Source
// SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

// ensure there is always a way to go home
document.addEventListener("DOMContentLoaded", () => {
  const navPath = document.getElementById("nav-path");
  if (navPath) {
    const ul = navPath.querySelector("ul");
    if (ul && ul.children.length === 1) {
      const newLi = document.createElement("li");
      newLi.className = "navelem";
      const homeLink = document.createElement("a");
      homeLink.href = "index.html";
      homeLink.className = "el";
      homeLink.textContent = "simd";
      newLi.appendChild(homeLink);
      ul.insertBefore(newLi, ul.firstChild);
    }
  }
});
