//
//  BeanDevice.h
//  BleArduino
//
//  Created by Raymond Kampmeier on 1/16/14.
//  Copyright (c) 2014 Punch Through Design. All rights reserved.
//

#if TARGET_OS_IPHONE
#import <CoreBluetooth/CoreBluetooth.h>
#else
#import <IOBluetooth/IOBluetooth.h>
#endif

#define ARDUINO_OAD_GENERIC_TIMEOUT_SEC 6
#define TPDBeanErrorDomain @"TPDBeanErrorDomain"

@class PTDBeanRadioConfig;
@class PTDBeanManager;
@protocol PTDBeanDelegate;

/**
 *  Error states returned by PTDBeanDelegate bean:error:
 
 */
typedef NS_ENUM(NSInteger, BeanErrors) {
    /**
     *  An input argument was invalid
     */
    BeanErrors_InvalidArgument = 0,
    /**
     *  The bean is not connected
     */
    BeanErrors_NotConnected
};

/**
 *  Represents the state of the Bean connection
 */
typedef NS_ENUM(NSInteger, BeanState) {
    /**
     *  Used for initialization and unknown error states
     */
    BeanState_Unknown = 0,
    /**
     *  Bean has been discovered by a central
     */
    BeanState_Discovered,
    /**
     *  Bean is attempting to connect with a central
     */
    BeanState_AttemptingConnection,
    /**
     *  Bean is undergoing validation
     */
    BeanState_AttemptingValidation,
    /**
     *  Bean is connected
     */
    BeanState_ConnectedAndValidated,
    /**
     *  Bean is disconnecting
     */
    BeanState_AttemptingDisconnection
};

/**
 *  docset does not work here
 */
typedef struct {
    double x;
    double y;
    double z;
} PTDAcceleration;

/**
 *  Transmission power levels availabe to the Bean
 */
typedef NS_ENUM(NSUInteger, PTDTxPower_dB) {
    /**
     *  4db. Do this to maximize your tranmission strength.
     */
    PTDTxPower_4dB = 0,
    /**
     *  0db. This is the default value.
     */
    PTDTxPower_0dB,
    /**
     *  -6db
     */
    PTDTxPower_neg6dB,
    /**
     *  -23db. Use this to maximize power savings.
     */
    PTDTxPower_neg23dB
};

/**
   An PTDBean object represents a Light Blue Bean that gives access to setting and retrieving of Arduino attributes, such as the name, temperature, accelerometer, look at Other Methods below for more.

   Implement the PTDBeanDelegate for receiving messages sent from the Bean.

   See [BeanXcodeWorkspace](http://www.punchthrough.com) for more examples.
 */

@interface PTDBean : NSObject
/**
 *  The delegate object for the Bean. The Bean asynchronously call methods on this delegate on the main thread.
 *  @see PTDBeanDelegate
 */
@property (nonatomic, weak) id<PTDBeanDelegate> delegate;
/**
 * The date the Bean firmware was programmed.
 */
@property (nonatomic, strong) NSDate *dateProgrammed;
/**
 *  The name of the Arduino Sketch used to program the Bean firmware.
 */
@property (nonatomic, strong) NSString *sketchName;

/**
 *  The Peripheral identifier. 
 *  For more info, refer to the [Apple identifier documentation](https://developer.apple.com/library/ios/documentation/CoreBluetooth/Reference/CBPeripheral_Class/translated_content/CBPeripheral.html#//apple_ref/occ/instp/CBPeripheral/identifier)
 */
-(NSUUID*)identifier;
/**
 *  The Peripheral name.
 *  For more info, refer to the [Apple name documentation](https://developer.apple.com/library/ios/documentation/CoreBluetooth/Reference/CBPeripheral_Class/translated_content/CBPeripheral.html#//apple_ref/occ/instp/CBPeripheral/name)
 */
-(NSString*)name;
/**
 *  The Peripheral RSSI.
 *  For more info, refer to the [Apple RSSI documentation](https://developer.apple.com/library/ios/documentation/CoreBluetooth/Reference/CBPeripheral_Class/translated_content/CBPeripheral.html#//apple_ref/occ/instp/CBPeripheral/RSSI)
 */
-(NSNumber*)RSSI;
/**
 *  The State of the bean. For example, check for BeanState_AttemptingConnection to see if your bean is connected.
 
     if (self.bean.state == BeanState_Discovered) {
        NSLog(@"Bean discovered, try connecting");
     }
     else if (self.bean.state == BeanState_ConnectedAndValidated) {
        NSLog(@"Bean connected, try calling an api");
     }
 
    @see BeanState
 */
-(BeanState)state;
/**
 *  A dictionary containing [CBAdvertisementDataLocalNameKey](https://developer.apple.com/library/ios/documentation/CoreBluetooth/Reference/CBCentralManagerDelegate_Protocol/translated_content/CBCentralManagerDelegate.html)
 */
-(NSDictionary*)advertisementData;
/**
 *  The last time this bean was discovered by a central.
 */
-(NSDate*)lastDiscovered;
/**
 *  The version of the Bean firmware.
 */
-(NSString*)firmwareVersion;
/**
 *  Used to create and manage Beans.
 *
 *  @see PTDBeanManager
 */
-(PTDBeanManager*)beanManager;
/**
 *  Reads the Arduino Sketch.
 *  @see [PTDBeanDelegate bean:didUpdateSketchName:dateProgrammed:crc32:]
 */
-(void)readArduinoSketchInfo;
/**
 *  Programs the Arduino with a hex file.
 *
 *  @param hexImage the hexImage for setting the firmware
 *  @param name the name of the file
 *  @see [PTDBeanDelegate bean:didProgramArduinoWithError:]
 */
-(void)programArduinoWithRawHexImage:(NSData*)hexImage andImageName:(NSString*)name;
/**
 *  A request to have Data sent from the Bean to this app.
 *
 *  @param length The size of the data to send.
 *  @see [PTDBeanDelegate bean:didUpdateLoopbackPayload:]
 */
-(void)sendLoopbackDebugMessage:(NSInteger)length;
/**
 *  Sends data to the Bean over a serial port.
 *  @param data data to send over the serial port
 *  @see [PTDBeanDelegate bean:serialDataReceived:]
 */
-(void)sendSerialData:(NSData*)data;
/**
 *  Sends a NSString over a serial port
 *  @param string string which is converted to NSData for sending over the serial port
 *  @see [PTDBeanDelegate bean:serialDataReceived:]
 */
-(void)sendSerialString:(NSString*)string;
/**
 Reads the Beans Accelerometer.
    
    // let the bean know we implment PTDBeanDelegate
    self.bean.delegate = self;
    // ask the bean for the acceleration data
    [self.bean readAccelerationAxis];
    
    // check for the bean to send it back
    -(void)bean:(PTDBean*)bean didUpdateAccelerationAxes:(PTDAcceleration)acceleration {
        NSString *msg = [NSString stringWithFormat:@"x:%f y:%f z:%f", acceleration.x,acceleration.y,acceleration.z];
        NSLog(@"%@", msg);
    }

 @see [PTDBeanDelegate bean:didUpdateAccelerationAxes:]
 */
-(void)readAccelerationAxis;
/**
 *  Sets the Led Color
 *  @param UIColor* Color object which is used to set the Led
 *  @see [PTDBeanDelegate bean:didUpdateLedColor:]
 */
#if TARGET_OS_IPHONE
-(void)setLedColor:(UIColor*)color;
#else
-(void)setLedColor:(NSColor*)color;
#endif
/**
 *  Reads the Led Color.
 *  @see [PTDBeanDelegate bean:didUpdateLedColor:]
 */
-(void)readLedColor;
/**
   Sets the Scratch Number with data. Think of it as temporary storage for.
 
    // set the scratch bank, 1-5
    int scratchNumber = 1
    // set the scratch data
    [self.bean setScratchNumber:scratchNumber withValue:[@"scratchdata" dataUsingEncoding:NSUTF8StringEncoding]];
    // after some time, ask for it back
    [self.bean readScratchBank:scratchNumber];
 
    // check the delegate value
    -(void)bean:(PTDBean *)bean didUpdateScratchNumber:(NSNumber *)number withValue:(NSData *)data {
      NSString* str = [NSString stringWithUTF8String:[data bytes]];
      NSString *msg = [NSString stringWithFormat:@"received scratch number:%@ scratch:%@", number, str];
      NSLog(@"%@", msg);
    }
 
 *  @param scratchNumber can be a value 1-5
 *  @param value         up to 20 bytes
 */
-(void)setScratchNumber:(NSInteger)scratchNumber withValue:(NSData*)value;
/**
 *  Reads the scratch bank.
 *
 *  @param bank can be a value 1-5
 *  @see [PTDBeanDelegate bean:didUpdateScratchNumber:withValue:]
 */
-(void)readScratchBank:(NSInteger)bank;
/**
 *  Reads the temperature.
 *  @see [PTDBeanDelegate bean:didUpdateTemperature:]
 */
-(void)readTemperature;
/**
 *  Sets the pairing pin.
 *
 *  @param pinCode the code to set the pairing pin
 */
-(void)setPairingPin:(UInt16)pinCode;
/**
 *  Reads the Radio Configuration.
 *  @see [PTDBeanDelegate bean:didUpdateRadioConfig:]
 */
-(void)readRadioConfig;
/**
 *  Sets the Radio Config.
 *  @param config
 */
-(void)setRadioConfig:(PTDBeanRadioConfig*)config;
/**
 *  Updates Firmware with a path to the hex file.
 *
 *  @param imageApath XXX
 *  @param imageBpath XXX
 *
 *  @return YES if successful
 */
-(BOOL)updateFirmwareWithImageAPath:(NSString*)imageApath andImageBPath:(NSString*)imageBpath;
@end

/**
 Delegates of a PTDBean object should implement this protocol.
    
    Example:
    // assigning yourself as the delgate
    self.bean.delegate = self;
    // ask the bean for the color of its led
    [self.bean readLedColor]
    
    // wait for the delegate callback
    - (void)bean:(PTDBean *)bean didUpdateLedColor:(UIColor *)color {
        NSLog(@"%@", color);
    }
 See [BeanXcodeWorkspace](http://www.punchthrough.com) for more examples.
 */
@protocol PTDBeanDelegate <NSObject>

@optional
/**
 * Sent when an error occurs
   @param bean  the bean that made the request
   @param error refer to BeanErrors for the list of error codes.
 
    if (error.code == BeanErrors_InvalidArgument) {
      UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Invalid Argument" message:[error localizedDescription]
        delegate:nil cancelButtonTitle:nil otherButtonTitles:@"Ok", nil];
      [alert show];
    }
    else {
       // do something else
    }
 */
-(void)bean:(PTDBean*)bean error:(NSError*)error;
/**
 * Sent when an error occurs during Arduino Programming
 @param bean  the bean that made the request
 @param error contains error. Refer to discussion
 
     NSError codes
     typedef enum {
       BeanState_Unknown Used // for initialization and unknown error states
       BeanState_Discovered Bean // has been discovered by a central
       BeanState_AttemptingConnection // Bean is attempting to connect with a central
       BeanState_AttemptingValidation // Bean is undergoing validation
       BeanState_ConnectedAndValidated // Bean is connected
       BeanState_AttemptingDisconnection // Bean is disconnecting
     } BeanState
     
     NSError domain TPDBeanErrorDomain
     Refer to the NSError localizedDescription for additional error information.
 
 */
-(void)bean:(PTDBean*)bean didProgramArduinoWithError:(NSError*)error;
/**
 *  Time remaining before the Arduino is finished programming.
 *
 *  @param bean               the Bean being programmed
 *  @param seconds            the remaining number of seconds
 *  @param percentageComplete the percentage already programmed
 */
-(void)bean:(PTDBean*)bean ArduinoProgrammingTimeLeft:(NSNumber*)seconds withPercentage:(NSNumber*)percentageComplete;
/**
 *  Serial data received from the Bean
 *
 *  @param bean the Bean sending the serial data
 *  @param data the data sent from the Bean
 */
-(void)bean:(PTDBean*)bean serialDataReceived:(NSData*)data;
/**
 *  Message from the Bean that the pairing in has been updated
 *
 *  @param bean    the Bean being updated
 *  @param pinCode the code used to update the bean
 */
-(void)bean:(PTDBean*)bean didUpdatePairingPin:(UInt16)pinCode;
#if TARGET_OS_IPHONE
/**
 *  The Bean Led color
 *
 *  @param bean  the Bean being queried
 *  @param color the color returned
 */
-(void)bean:(PTDBean*)bean didUpdateLedColor:(UIColor*)color;
#else
/**
 *  The Bean Led color
 *
 *  @param bean the Bean being queried
 *  @param color the color returned
 */
-(void)bean:(PTDBean*)bean didUpdateLedColor:(NSColor*)color;
#endif
/**
 The Bean accelerometer readings
 
 @param bean         the Bean being queried
 @param acceleration The type of a structure containing 3-axis acceleration values, identical to [CMAcceleration](https://developer.apple.com/library/ios/documentation/coremotion/reference/CMAccelerometerData_Class/Reference/Reference.html#//apple_ref/doc/c_ref/CMAcceleration)
 
     typedef struct {
     double x;
     double y;
     double z;
     } PTDAcceleration;
 
 */
-(void)bean:(PTDBean*)bean didUpdateAccelerationAxes:(PTDAcceleration)acceleration;
/**
 *  The Bean temperature readings
 *
 *  @param bean            the bean being queried
 *  @param degrees_celsius the temperature in celsius
 */
-(void)bean:(PTDBean*)bean didUpdateTemperature:(NSNumber*)degrees_celsius;
/**
 *  The payload returned from a Bean after a loopback call
 *
 *  @param bean    the Bean being queried
 *  @param payload the loopback data
 */
-(void)bean:(PTDBean*)bean didUpdateLoopbackPayload:(NSData*)payload;
/**
 *  The Bean configuration
 *
 *  @param bean   the Bean being queried
 *  @param config the configuration of the bean
 *  @see PTDBeanRadioConfig
 */
-(void)bean:(PTDBean*)bean didUpdateRadioConfig:(PTDBeanRadioConfig*)config;
/**
 *  The Beans scratch characteristic.
 *
 *  @param bean   the Bean being queried
 *  @param number the scratch number
 *  @param data   the data stored in the scratch characteristic
 */
-(void)bean:(PTDBean*)bean didUpdateScratchNumber:(NSNumber*)number withValue:(NSData*)data;
/**
 Sent when an error occurs during a Firmware Upload
 @param bean  the Bean that made the request
 @param error the error can be interpreted by its error code and description
 
     NSError codes
     typedef enum {
       BeanState_Unknown Used // for initialization and unknown error states
       BeanState_Discovered Bean // has been discovered by a central
       BeanState_AttemptingConnection // Bean is attempting to connect with a central
       BeanState_AttemptingValidation // Bean is undergoing validation
       BeanState_ConnectedAndValidated // Bean is connected
       BeanState_AttemptingDisconnection // Bean is disconnecting
     } BeanState
     
     NSError domain TPDBeanErrorDomain
     Refer to the NSError localizedDescription for additional error information.
 
 */
-(void)bean:(PTDBean*)bean completedFirmwareUploadWithError:(NSError*)error;
/**
 *  Time remaining before the firmware has completed uploading
 *
 *  @param bean               the Bean being updated
 *  @param seconds            the remaining seconds for the upload
 *  @param percentageComplete the percentage of the upload complete
 */
-(void)bean:(PTDBean*)bean firmwareUploadTimeLeft:(NSNumber*)seconds withPercentage:(NSNumber*)percentageComplete;
/**
 *  Message from the Bean that a sketch has been programmed
 *
 *  @param bean the Bean being programmed
 *  @param name the name of the sketch
 *  @param date the date of the programming
 *  @param crc  the cyclic redundancy check
 */
-(void)bean:(PTDBean*)bean didUpdateSketchName:(NSString*)name dateProgrammed:(NSDate*)date crc32:(UInt32)crc;
@end