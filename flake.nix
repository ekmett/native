# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
{
  description = "Native C++26 SIMD and instruction interfaces";

  inputs = {
    nixpkgs.url = "tarball+https://codeload.github.com/NixOS/nixpkgs/tar.gz/e7439b6b14ad3cc35d05608ebca9bce01a25f5f8";
    hint = {
      url = "git+https://github.com/ekmett/hint?ref=main&shallow=1";
      flake = false;
    };
  };

  outputs = { self, nixpkgs, hint, ... }:
    let
      systems = [ "x86_64-linux" "aarch64-linux" ];
      forAllSystems = nixpkgs.lib.genAttrs systems;
      packagesFor = system:
        let
          pkgs = import nixpkgs { inherit system; };
          llvm = pkgs.llvmPackages_23;
          hintPackage = pkgs.stdenvNoCC.mkDerivation {
            pname = "hint";
            version = "0.1.0";
            src = hint;
            nativeBuildInputs = [ pkgs.cmake pkgs.ninja ];
            cmakeFlags = [ "-DHINT_BUILD_TESTS=OFF" "-DHINT_BUILD_DOCS=OFF" ];
          };
          native = llvm.stdenv.mkDerivation {
            pname = "native";
            version = "0.0.1";
            src = pkgs.lib.fileset.toSource {
              root = ./.;
              fileset = pkgs.lib.fileset.unions [
                ./CMakeLists.txt ./LICENSE.spdx ./LICENSE.md
                ./THIRD-PARTY-NOTICES.md ./src ./etc/cmake ./tests/api
              ];
            };
            nativeBuildInputs = [ pkgs.cmake pkgs.ninja llvm.clang-tools ];
            propagatedBuildInputs = [ hintPackage ];
            cmakeFlags = [
              "-DNATIVE_BUILD_TESTS=OFF"
              "-DFETCHCONTENT_FULLY_DISCONNECTED=ON"
            ];
            # Test the installed package, not a second in-tree library build.
            doInstallCheck = true;
            installCheckPhase = ''
              runHook preInstallCheck
              cmake -S "$src/tests/api" -B consumer -G Ninja \
                -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="$out;${hintPackage}"
              cmake --build consumer --parallel "$NIX_BUILD_CORES"
              ctest --test-dir consumer --output-on-failure
              runHook postInstallCheck
            '';
            meta = {
              description = "C++26 SIMD values and native instruction interfaces";
              homepage = "https://github.com/ekmett/native";
              license = with pkgs.lib.licenses; [ bsd2 asl20 ];
              platforms = systems;
            };
          };
        in { inherit native; default = native; };
    in {
      packages = forAllSystems packagesFor;
      checks = forAllSystems (system: { inherit (self.packages.${system}) native; });
      devShells = forAllSystems (system:
        let pkgs = import nixpkgs { inherit system; };
        in {
          default = (pkgs.mkShell.override { stdenv = pkgs.llvmPackages_23.stdenv; }) {
            inputsFrom = [ self.packages.${system}.native ];
          };
        });
    };
}
