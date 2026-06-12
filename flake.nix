{
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs =
    {
      self,
      nixpkgs,
      flake-utils,
      ...
    }:
    flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        llvmPkgs = pkgs.llvmPackages_latest;
        stdenv = llvmPkgs.stdenv;
        llvmDir = "${llvmPkgs.llvm.dev}/lib/cmake/llvm";

        package = {
          pname = "cuhpiler";
          version = "0.1.0";
          src = ./.;
          nativeBuildInputs = with pkgs; [
            cmake
            ninja
            pkg-config
          ];
          buildInputs = with pkgs; [
            llvmPkgs.llvm
          ];
          cmakeFlags = [ "-DLLVM_DIR=${llvmDir}" ];
        };
      in
      {
        packages = {
          default = stdenv.mkDerivation (
            package
            // {
              cmakeBuildType = "Release";
            }
          );
          debug = stdenv.mkDerivation (
            package
            // {
              cmakeBuildType = "Debug";
              dontStrip = true;
            }
          );
        };

        devShells.default = pkgs.mkShell.override { inherit stdenv; } {
          inputsFrom = [ self.packages.${system}.default ];

          packages = with pkgs; [
            clang-tools
            lldb
          ];

          LLVM_DIR = llvmDir;
        };
      }
    );
}
