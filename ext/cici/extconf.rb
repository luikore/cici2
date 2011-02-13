require "mkmf"

inc_path = File.expand_path "#{File.dirname(__FILE__)}/../include"
$INCFLAGS << " -I\"#{inc_path}\""
if RUBY_PLATFORM =~ /mingw/
  $LIBS << " -lgdi32 -luser32 -lkernel32 -lcomdlg32 -lcomctl32"
elsif RUBY_PLATFORM =~ /mswin/
  $CPPFLAGS << '/EHsc' # disable warning on exceptions
else
  raise 'can not recognize ruby platform'
end

create_makefile 'ext'
