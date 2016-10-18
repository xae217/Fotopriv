
//
// This file is auto-generated. Please don't modify it!
//
package org.opencv.video;



// C++: class FarnebackOpticalFlow
//javadoc: FarnebackOpticalFlow
public class FarnebackOpticalFlow extends DenseOpticalFlow {

    protected FarnebackOpticalFlow(long addr) { super(addr); }


    @Override
    protected void finalize() throws Throwable {
        delete(nativeObj);
    }



    // native support for java finalize()
    private static native void delete(long nativeObj);

}
