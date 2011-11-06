TEST_FILES = %w{s3_settings.c}

def compile(file)
  `gcc -DTESTING -Wall -Wextra -g -o test_exec src/#{file}`
end

def execute(file)
  `./src/test_exec`
end

def convert_to_hash(output)
  output.each_line.reduce({}) do |h, line|
    key = line.split[0]
    val = line.split[1]
    h.merge(key => val)
  end
end

def test_s3_settings_with_no_existing_file
  compile('s3_settings.c') 
  output = exec('s3_settings.c')
    
end
