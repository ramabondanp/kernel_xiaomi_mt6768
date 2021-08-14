# AnyKernel3 Ramdisk Mod Script
# osm0sis @ xda-developers

## AnyKernel setup
# begin properties
properties() { '
kernel.string=Genom Kernel by rama982@telegram
do.devicecheck=0
do.modules=0
do.systemless=1
do.cleanup=1
do.cleanuponabort=0
device.name1=
supported.versions=
supported.patchlevels=
'; } # end properties

# shell variables
block=/dev/block/platform/bootdevice/by-name/boot;
is_slot_device=0;
ramdisk_compression=auto;

## AnyKernel methods (DO NOT CHANGE)
# import patching functions/variables - see for reference
. tools/ak3-core.sh;

<< ////

# THIS CODE BELOW IS COMMENTED

# Key select start
ui_print " "
ui_print "Do not touch your screen until finished!"
ui_print " "
ui_print "PRESS ANY VOLUME BUTTON FIRST!"

INSTALLER=$(pwd)
KEYCHECK=$INSTALLER/tools/keycheck
chmod 755 $KEYCHECK

keytest() {
  (/system/bin/getevent -lc 1 2>&1 | /system/bin/grep VOLUME | /system/bin/grep " DOWN" > $INSTALLER/events) || return 1
  return 0
}

choose() {
  #note from chainfire @xda-developers: getevent behaves weird when piped, and busybox grep likes that even less than toolbox/toybox grep
  while true; do
    /system/bin/getevent -lc 1 2>&1 | /system/bin/grep VOLUME | /system/bin/grep " DOWN" > $INSTALLER/events
    if (`cat $INSTALLER/events 2>/dev/null | /system/bin/grep VOLUME >/dev/null`); then
      break
    fi
  done
  if (`cat $INSTALLER/events 2>/dev/null | /system/bin/grep VOLUMEUP >/dev/null`); then
    return 0
  else
    return 1
  fi
}

chooseold() {
  # Calling it first time detects previous input. Calling it second time will do what we want
  $KEYCHECK
  $KEYCHECK
  SEL=$?
  if [ "$1" == "UP" ]; then
    UP=$SEL
  elif [ "$1" == "DOWN" ]; then
    DOWN=$SEL
  elif [ $SEL -eq $UP ]; then
    return 0
  elif [ $SEL -eq $DOWN ]; then
    return 1
  else
    ui_print "   Vol key not detected!"
    abort "   Use name change method in TWRP"
  fi
}

if [ -z $NEW ]; then
  if keytest; then
    FUNCTION=choose
  else
    FUNCTION=chooseold
    ui_print " "
    ui_print "- Vol Key Programming -"
    ui_print "   Press Volume Up Key: "
    $FUNCTION "UP"
    ui_print "   Press Volume Down Key: "
    $FUNCTION "DOWN"
  fi
  ui_print " "
  ui_print "- Select Option -"
  ui_print "   Do you want to enable RAM Extension feature? it will use 2GB of internal storage:"
  ui_print "   Volume Up = YES, Volume Down = NO"
  if $FUNCTION; then
    NEW=true
  else
    NEW=false
  fi
else
  ui_print "   Option specified in zipname!"
fi
# Key select end

# Select if ram ext aka swapfile will be enabled
if $NEW; then #true
  ui_print " "
  ui_print "Volume UP is pressed:"
  ui_print "RAM EXT Enabled!"
  ui_print " "
else #false
  echo "#empty" > $ramdisk/overlay.d/sbin/init.genom.sh;
  rm /data/swapfile
  ui_print " "
  ui_print "Volume DOWN is pressed:"
  ui_print "RAM EXT Disabled!"
  ui_print " "
fi

////

## AnyKernel file attributes
# set permissions/ownership for included ramdisk files
set_perm_recursive 0 0 755 644 $ramdisk/*;
set_perm_recursive 0 0 750 750 $ramdisk/init* $ramdisk/sbin;
set_perm_recursive 0 0 755 644 $ramdisk/overlay.d/*;
set_perm_recursive 0 0 750 750 $ramdisk/overlay.d/init* $ramdisk/overlay.d/sbin;

## AnyKernel boot install
dump_boot;

# begin ramdisk changes

# patch_cmdline androidboot.selinux androidboot.selinux=permissive

# end ramdisk changes

write_boot;
## end boot install


# shell variables
#block=vendor_boot;
#is_slot_device=1;
#ramdisk_compression=auto;

# reset for vendor_boot patching
#reset_ak;


## AnyKernel vendor_boot install
#split_boot; # skip unpack/repack ramdisk since we don't need vendor_ramdisk access

#flash_boot;
## end vendor_boot install


