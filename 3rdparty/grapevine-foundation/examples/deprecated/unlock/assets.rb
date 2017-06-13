#!/usr/bin/env ruby

files = Dir["tmp/*.FBX"]
files.each do|file|
    prefix = file.split('.').first
    file = file.gsub(' ', '\ ')
    prefix = prefix.gsub(' ', '\ ')

    targetFile = prefix.split('/').last
    convertCommand = "barrel " + file + " android/unlock/assets/" + targetFile + ".barrel"
    print "converting: " + file + "(" + convertCommand + ")\n"
    system(convertCommand)
end
