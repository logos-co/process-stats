{
  description = "Process statistics library for monitoring CPU and memory usage";

  inputs = {
    logos-nix.url = "github:logos-co/logos-nix";
    nixpkgs.follows = "logos-nix/nixpkgs";
  };

  outputs = { self, nixpkgs, logos-nix }:
    let
      systems = [ "aarch64-darwin" "x86_64-darwin" "aarch64-linux" "x86_64-linux" ];
      forAllSystems = f: nixpkgs.lib.genAttrs systems (system: f {
        inherit system;
        pkgs = import nixpkgs { inherit system; };
      });
    in
    {
      packages = forAllSystems ({ pkgs, system }: 
        let
          # Common configuration
          common = import ./nix/default.nix { inherit pkgs; };
          src = ./.;
          
          # Shared build that compiles everything
          build = import ./nix/build.nix { inherit pkgs common src; };
          
          # Individual package components (reference the shared build)
          lib = import ./nix/lib.nix { inherit pkgs common build; };
          include = import ./nix/include.nix { inherit pkgs common src; };
          tests = import ./nix/tests.nix { inherit pkgs common build; };
          
          # Combined package
          process-stats = pkgs.symlinkJoin {
            name = "process-stats";
            paths = [ lib include ];
          };
        in
        {
          # Individual outputs
          process-stats-lib = lib;
          process-stats-include = include;
          process-stats-tests = tests;
          
          # Combined output
          process-stats = process-stats;
          
          # Default package
          default = process-stats;
        }
      );

      checks = forAllSystems ({ pkgs, system }:
        let
          testsPkg = self.packages.${system}.process-stats-tests;
        in
        {
          tests = pkgs.runCommand "process-stats-tests" {
            nativeBuildInputs = [ testsPkg ];
          } ''
            echo "Running process-stats tests..."
            ${testsPkg}/bin/process_stats_tests
            mkdir -p $out
            touch $out/.tests-passed
          '';
        }
      );

      devShells = forAllSystems ({ pkgs }: {
        default = pkgs.mkShell {
          nativeBuildInputs = [
            pkgs.cmake
            pkgs.ninja
            pkgs.pkg-config
          ];
          buildInputs = [
            pkgs.nlohmann_json
            pkgs.gtest
          ];
        };
      });
    };
}
