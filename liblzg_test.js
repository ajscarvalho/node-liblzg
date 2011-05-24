
var sys = require("sys");
var lzg = require("liblzg");

var text = "The quick brown fox jumps over the lazy dog! The quick brown fox jumps over the lazy dog!";
sys.debug("Original Text: " + text + " (" + text.length + ")");

var compressed = lzg.compress(text, 1, 5);
sys.debug("Compressed Text: " + compressed + " (" + compressed.length + ")");

var decompressed = lzg.decompress(compressed);
sys.debug("Decompressed Text: " + decompressed);


