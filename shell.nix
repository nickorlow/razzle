let
  pkgs' = (import <nixpkgs> {}).pkgsCross.i686-embedded;
  pkgs = (import <nixpkgs> {});
in
  pkgs'.mkShell {
    nativeBuildInputs = [ 
        pkgs.cdrkit
        pkgs.qemu
        pkgs.xxd
    ];
}
