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

        package = {
          pname = "cuhpiler";
          version = "0.1.0";
          src = ./.;
          nativeBuildInputs = with pkgs; [
            cmake
          ];
          buildInputs = with pkgs; [
          ];
        };
      in
      {
        packages = {
          default = pkgs.clangStdenv.mkDerivation package;
          debug = pkgs.clangStdenv.mkDerivation (
            package
            // {
              cmakeBuildType = "Debug";
              dontStrip = true;
            }
          );
        };

        devShells.default = pkgs.mkShell {
          inputsFrom = [ self.packages.${system}.default ];

          packages = with pkgs; [
            clang-tools
            lldb
          ];
        };
      }
    );
}
